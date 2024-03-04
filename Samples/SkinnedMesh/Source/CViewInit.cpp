#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true),
	m_character01(NULL),
	m_character02(NULL)
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
	camera->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	camera->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	ITexture* skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome* skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// reflection probe
	CGameObject* reflectionProbeObj = zone->createEmptyObject();
	CReflectionProbe* reflection = reflectionProbeObj->addComponent<CReflectionProbe>();
	reflection->loadStaticTexture("Common/Textures/Sky/PaperMill");

	// 3D grid
	CGameObject* grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// load dae animation
	CAnimationManager* animManager = CAnimationManager::getInstance();
	CAnimationClip* clip1 = animManager->loadAnimation("SampleModels/MixamoCharacter/Hip_Hop_Dancing.dae");
	CAnimationClip* clip2 = animManager->loadAnimation("SampleModels/MixamoCharacter/Samba_Dancing.dae");

	// skinned mesh
	CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel("SampleModels/MixamoCharacter/Ch17_nonPBR.dae", "SampleModels/MixamoCharacter/textures");
	if (prefab != NULL)
	{
		// test exporter
		/*
		CMeshManager::getInstance()->exportModel(
			prefab->getEntities(),
			prefab->getNumEntities(),
			"../Assets/SkinnedMesh/Ch17_nonPBR.smesh");
		*/

		ArrayMaterial material = CMaterialManager::getInstance()->initDefaultMaterial(prefab);
		if (material.size() == 2)
		{
			// body
			material[1]->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkin.xml");
			material[1]->autoDetectLoadTexture();

			// hair
			material[0]->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkinAlpha.xml");
			material[0]->autoDetectLoadTexture();
		}

		// CHARACTER 01
		// ------------------------------------------

		// create character 01 object
		m_character01 = zone->createEmptyObject();

		// load skinned mesh character 01
		CRenderMesh* renderMesh1 = m_character01->addComponent<CRenderMesh>();
		renderMesh1->initFromPrefab(prefab);
		renderMesh1->initMaterial(material);

		// apply animation to character 01
		CAnimationController* animController1 = m_character01->addComponent<CAnimationController>();
		CSkeleton* skeleton1 = animController1->createSkeleton();
		skeleton1->setAnimation(clip1, true);

		// set position for character 01
		m_character01->getTransformEuler()->setPosition(core::vector3df(-1.0f, 0.0f, 0.0f));

		// set sh lighting
		CIndirectLighting* indirectLighting = m_character01->addComponent<CIndirectLighting>();
		indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);


		// CHARACTER 02
		// ------------------------------------------

		// create character 02 object
		m_character02 = zone->createEmptyObject();

		// load skinned mesh character 02
		CRenderMesh* renderMesh2 = m_character02->addComponent<CRenderMesh>();
		renderMesh2->initFromPrefab(prefab);
		renderMesh2->initMaterial(material);

		// apply animation to character 02
		CAnimationController* animController2 = m_character02->addComponent<CAnimationController>();
		CSkeleton* skeleton2 = animController2->createSkeleton();
		skeleton2->setAnimation(clip2, true);

		// set position for character 02
		m_character02->getTransformEuler()->setPosition(core::vector3df(1.0f, 0.0f, 0.0f));

		// set sh lighting
		indirectLighting = m_character02->addComponent<CIndirectLighting>();
		indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);
	}

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();
	CForwardRP* fwrp = new CForwardRP();
	fwrp->initRender(w, h);

	context->setCustomRP(fwrp);
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
		listBundles.push_back("SampleModelsResource.zip");
		listBundles.push_back(getApplication()->getTexturePackageName("SampleModels").c_str());

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
#if defined(WINDOWS_STORE)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#elif defined(MACOS)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#else
			fileSystem->addFileArchive(r, false, false);
#endif
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
		CCamera* camera = context->getActiveCamera();

		if (m_bakeSHLighting == true)
		{
			m_bakeSHLighting = false;

			m_character01->setVisible(false);
			m_character02->setVisible(false);

			CGameObject* bakeCameraObj = scene->getZone(0)->createEmptyObject();
			CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
			scene->updateAddRemoveObject();

			core::vector3df pos(0.0f, 0.0f, 0.0f);

			core::vector3df normal = CTransform::s_oy;
			core::vector3df tangent = CTransform::s_ox;
			core::vector3df binormal = normal.crossProduct(tangent);
			binormal.normalize();

			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);
			Lightmapper::CSH9 sh = lm->bakeAtPosition(
				bakeCamera,
				rp,
				scene->getEntityManager(),
				pos,
				normal, tangent, binormal);

			// apply indirect lighting
			m_character01->getComponent<CIndirectLighting>()->setSH(sh.getValue());
			m_character02->getComponent<CIndirectLighting>()->setSH(sh.getValue());

			m_character01->setVisible(true);
			m_character02->setVisible(true);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
