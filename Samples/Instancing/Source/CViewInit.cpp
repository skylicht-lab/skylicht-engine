#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "GridPlane/CGridPlane.h"
#include "LOD/CLOD.h"
#include "SkyDome/CSkyDome.h"
#include "Primitive/CPlane.h"
#include "CRotateComponent.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0)
{

}

CViewInit::~CViewInit()
{
	m_guiObject->remove();
	delete m_font;
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

	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	// camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	CCamera* camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(10.0f, 5.0f, 10.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// sky
	ITexture* skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/MonValley.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome* skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(1.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, Transform::Oy);

	CMeshManager* meshManager = CMeshManager::getInstance();
	CMaterialManager* materialMgr = CMaterialManager::getInstance();

	CGameObject* windTurbine = NULL;
	CGameObject* windTurbineBlades = NULL;

	CEntityPrefab* prefabWindTurbine = meshManager->loadModel("SampleModels/WindTurbine/WindTurbine.fbx", NULL, true);
	CEntityPrefab* prefabBlades = meshManager->loadModel("SampleModels/WindTurbine/WindTurbine_Blades.fbx", NULL, true);

	std::vector<std::string> textureFolders;
	ArrayMaterial& winTurbineMaterials = materialMgr->loadMaterial("SampleModels/WindTurbine/WindTurbine.mat", true, textureFolders);
	ArrayMaterial& bladeTurbineMaterials = materialMgr->loadMaterial("SampleModels/WindTurbine/WindTurbine_Blades.mat", true, textureFolders);
	ArrayMaterial& sandMaterials = materialMgr->loadMaterial("SampleModels/WindTurbine/Sand.mat", true, textureFolders);

	SColor ambientColor(255, 190, 220, 250);
	float space = 50.0f;
	int numObjectInRow = 40;

	// total object = numObjectInRow * numObjectInRow * (turbine + blade) * numLOD

	CGameObject* ground = zone->createEmptyObject();
	CPlane* plane = ground->addComponent<CPlane>();
	plane->removeAllEntities();
	plane->setInstancing(true);
	plane->setEnableNormalMap(true);
	plane->setCustomMaterial(sandMaterials[0]);

	int n = numObjectInRow / 2;
	for (int x = -n; x < n; x++)
	{
		for (int z = -n; z < n; z++)
		{
			if (prefabWindTurbine)
			{
				windTurbine = zone->createEmptyObject();
				windTurbine->setStatic(true);

				CRenderMesh* renderer = windTurbine->addComponent<CRenderMesh>();
				// Just init render entity node, that contain meshbuffer
				renderer->enableOptimizeForRender(true);
				renderer->initFromPrefab(prefabWindTurbine);
				renderer->initMaterial(winTurbineMaterials);
				renderer->enableInstancing(true);

				CIndirectLighting* indirect = windTurbine->addComponent<CIndirectLighting>();
				indirect->setIndirectLightingType(CIndirectLighting::AmbientColor);
				indirect->setAmbientColor(ambientColor);

				windTurbine->addComponent<CLOD>();

				// rotate the turbine
				windTurbine->getTransformEuler()->setRotation(core::vector3df(0.0f, -90.0f, 0.0f));
			}

			if (windTurbine && prefabBlades)
			{
				windTurbineBlades = zone->createEmptyObject();

				// attach blade to turbine position
				CTransformEuler* transform = windTurbineBlades->getTransformEuler();
				transform->attachTransform(windTurbine->getEntity());
				transform->setPosition(core::vector3df(-1.59f, 38.0f, 0.0f));

				CRenderMesh* renderer = windTurbineBlades->addComponent<CRenderMesh>();
				// Just init render entity node, that contain meshbuffer
				renderer->enableOptimizeForRender(true);
				renderer->initFromPrefab(prefabBlades);
				renderer->initMaterial(bladeTurbineMaterials);
				renderer->enableInstancing(true);

				CIndirectLighting* indirect = windTurbineBlades->addComponent<CIndirectLighting>();
				indirect->setIndirectLightingType(CIndirectLighting::AmbientColor);
				indirect->setAmbientColor(ambientColor);

				windTurbineBlades->addComponent<CLOD>();

				core::vector3df randomRot(os::Randomizer::frand() * 360.0f, 0.0f, 0.0f);
				windTurbineBlades->getTransformEuler()->setRotation(randomRot);

				CRotateComponent* rotate = windTurbineBlades->addComponent<CRotateComponent>();
				rotate->setRotate(0.1f, 0.0f, 0.0f);
			}

			// wind
			windTurbine->getTransformEuler()->setPosition(core::vector3df(x * space, 0.0f, z * space));

			// ground & set static ambient color
			CEntity* entity = plane->addPrimitive(core::vector3df(x * space, 0.0f, z * space), core::vector3df(), core::vector3df(50.0f, 1.0f, 50.0f));
			CIndirectLightingData* indirectLighting = GET_ENTITY_DATA(entity, CIndirectLightingData);
			indirectLighting->Type = CIndirectLightingData::AmbientColor;
			indirectLighting->Color = ambientColor;
		}
	}

	// save to context
	CContext* context = CContext::getInstance();
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->getPostProcessorPipeline()->enableAutoExposure(false);
	context->getPostProcessorPipeline()->setManualExposure(1.0f);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);	
	context->setDirectionalLight(directionalLight);
}

void CViewInit::onDestroy()
{

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
		listBundles.push_back("SampleModels.zip");
		listBundles.push_back("SampleModelsResource.zip");
		listBundles.push_back(getApplication()->getTexturePackageName("SampleModels"));

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
	if (m_initState != Finished)
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
