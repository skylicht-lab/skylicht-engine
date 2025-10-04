#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"
#include "Transform/CWorldTransformSystem.h"

#include "Primitive/CPlane.h"
#include "SkySun/CSkySun.h"


CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true),
	m_plane(NULL),
	m_map(NULL)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char* name)
{
	return getApplication()->getBuiltInPath(name);
}

void CViewInit::onInit()
{
	CBaseApp* app = getApplication();
	app->showDebugConsole();
	app->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->initSGForwarderShader();
	shaderMgr->initSGDeferredShader();

	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// init basic gui
	CZone* zone = CContext::getInstance()->initScene()->createZone();
	m_guiObject = zone->createEmptyObject();
	CCanvas* canvas = m_guiObject->addComponent<CCanvas>();

	// load font
	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	// create text
	m_textInfo = canvas->createText(m_font);
	m_textInfo->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	m_textInfo->setText(L"Init assets");

	// create gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);
	CContext::getInstance()->setGUICamera(guiCamera);
}

void CViewInit::initScene()
{
	CBaseApp* app = getApplication();

	// create a scene
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	// camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	CCamera* camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(0.0f, 5.0f, 10.0f));
	camera->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	CSkySun* skySun = zone->createEmptyObject()->addComponent<CSkySun>();

	// reflection probe
	CGameObject* reflectionProbeObj = zone->createEmptyObject();
	CReflectionProbe* reflection = reflectionProbeObj->addComponent<CReflectionProbe>();
	reflection->loadStaticTexture("Common/Textures/Sky/PaperMill");

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, Transform::Oy);

	// plane
	m_plane = zone->createEmptyObject();
	m_plane->setName("Plane");
	m_plane->getTransformEuler()->setScale(core::vector3df(100.0f, 1.0f, 100.0f));

	CPlane* plane = m_plane->addComponent<CPlane>();
	plane->getMaterial()->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetersGrid.xml");

	// map
	CEntityPrefab* mapPrefab = CMeshManager::getInstance()->loadModel(COLLISION_MODEL, "");
	if (mapPrefab)
	{
		m_map = zone->createEmptyObject();
		m_map->setName("Map");

		CRenderMesh* renderMesh = m_map->addComponent<CRenderMesh>();
		renderMesh->initFromPrefab(mapPrefab);

		ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(mapPrefab);
		for (CMaterial* mat : materials)
		{
			mat->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetersGrid.xml");
		}
		renderMesh->initMaterial(materials);

		m_map->addComponent<CIndirectLighting>();
		m_map->setVisible(false);
	}

	// rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initRenderPipeline(w, h);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);
}

void CViewInit::onDestroy()
{
	m_guiObject->remove();
	delete m_font;
}

void CViewInit::onUpdate()
{
	CContext* context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();

		std::vector<std::string> listBundles;
		listBundles.push_back("Common.zip");
		listBundles.push_back("SampleGraphResource.zip");

#ifdef __EMSCRIPTEN__
		const char* filename = listBundles[m_downloaded].c_str();

		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL(filename, filename);
			m_getFile->download(CGetFileURL::Get);

			char log[512];
			sprintf(log, "Download asset: %s", filename);
			os::Printer::log(log);
		}
		else
		{
			char log[512];
			sprintf(log, "Download asset: %s - %d%%", filename, m_getFile->getPercent());
			m_textInfo->setText(log);

			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				// [bundles].zip
				fileSystem->addFileArchive(filename, false, false);

				if (++m_downloaded >= listBundles.size())
					m_initState = CViewInit::InitScene;
				else
				{
					delete m_getFile;
					m_getFile = NULL;
				}
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
	}
	}
#else

		for (std::string& bundle : listBundles)
		{
			const char* r = bundle.c_str();
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
		}

		m_initState = CViewInit::InitScene;
#endif
}
	break;
	case CViewInit::InitScene:
	{
		initScene();
		m_initState = CViewInit::Finished;
	}
	break;
	case CViewInit::Error:
	{
		// todo nothing with black screen
	}
	break;
	default:
	{
		CScene* scene = context->getScene();
		if (scene != NULL)
			scene->update();

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
	break;
}
}

void CViewInit::onRender()
{
	if (m_initState == CViewInit::Finished)
	{
		CContext* context = CContext::getInstance();
		CScene* scene = CContext::getInstance()->getScene();
		CBaseRP* rp = CContext::getInstance()->getRenderPipeline();

		if (m_bakeSHLighting == true)
		{
			m_bakeSHLighting = false;

			CZone* zone = scene->getZone(0);

			// light probe
			CGameObject* lightProbeObj = zone->createEmptyObject();
			CLightProbe* lightProbe = lightProbeObj->addComponent<CLightProbe>();
			lightProbeObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

			CGameObject* bakeCameraObj = scene->getZone(0)->createEmptyObject();
			CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
			scene->update();

			// bake light probe
			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);

			std::vector<CLightProbe*> probes;
			probes.push_back(lightProbe);

			lm->bakeProbes(probes, bakeCamera, rp, scene->getEntityManager());

			m_plane->setVisible(false);
			m_map->setVisible(false);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
