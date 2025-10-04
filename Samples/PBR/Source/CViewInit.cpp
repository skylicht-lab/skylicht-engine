#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "Primitive/CPlane.h"
#include "Primitive/CSphere.h"
#include "SkyDome/CSkyDome.h"

#define TEST_PBR
#define TEST_SG
#define TEST_MOBILE_SG

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true)
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
	shaderMgr->initPBRForwarderShader();
	shaderMgr->initSGForwarderShader();
	shaderMgr->initMobileSGShader();

	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// init basic gui
	CZone* zone = CContext::getInstance()->initScene()->createZone();
	m_guiObject = zone->createEmptyObject();
	CCanvas* canvas = m_guiObject->addComponent<CCanvas>();

	// load font
	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 100);

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
	camera->setPosition(core::vector3df(0.0f, 3.0f, 3.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// set mask for canvas text 3d
	// culling mask bit: 11
	camera->setCullingMask(3);

	// gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);
	guiCamera->setCullingMask(4);

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

	// helmet
	{
		m_helmet = zone->createEmptyObject();
		m_helmet->setName("helmet");

		// render mesh & init material
		std::vector<std::string> searchTextureFolders;
		CEntityPrefab* modelPrefab = CMeshManager::getInstance()->loadModel("SampleModels/DamagedHelmet/DamagedHelmet.dae", NULL, true, false);

		CRenderMesh* characterRenderer = m_helmet->addComponent<CRenderMesh>();
		characterRenderer->initFromPrefab(modelPrefab);

		ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(modelPrefab);
		materials[0]->changeShader("BuiltIn/Shader/PBR/Forward/PBR.xml");

		CTextureManager* textureManager = CTextureManager::getInstance();
		ITexture* albedoMap = textureManager->getTexture("SampleModels/DamagedHelmet/Default_albedo.jpg");
		ITexture* normalMap = textureManager->getTexture("SampleModels/DamagedHelmet/Default_normal.jpg");
		ITexture* rmaMap = textureManager->getTexture("SampleModels/DamagedHelmet/Default_metalRoughness.jpg");
		ITexture* emissiveMap = textureManager->getTexture("SampleModels/DamagedHelmet/Default_emissive.jpg");
		materials[0]->setUniformTexture("uTexAlbedo", albedoMap);
		materials[0]->setUniformTexture("uTexNormal", normalMap);
		materials[0]->setUniformTexture("uTexRMA", rmaMap);
		materials[0]->setUniformTexture("uTexEmissive", emissiveMap);
		characterRenderer->initMaterial(materials);

		// indirect lighting
		m_helmet->addComponent<CIndirectLighting>();
	}

#ifdef TEST_PBR
	{
		// PBR spheres
		int n = 5;
		core::vector3df offset(2.0f, 0.0f, -n / 2.0f);
		float distance = 1.0f;

		for (int i = 0; i <= n; i++)
		{
			for (int j = 0; j <= n; j++)
			{
				CGameObject* sphereObj = zone->createEmptyObject();

				CSphere* sphere = sphereObj->addComponent<CSphere>();

				CMaterial* material = sphere->getMaterial();
				material->changeShader("BuiltIn/Shader/PBR/Forward/PBRNoTexture.xml");

				float roughnessMetal[2];
				roughnessMetal[0] = (float)i / (float)n;
				roughnessMetal[1] = (float)j / (float)n;
				material->setUniform2("uRoughnessMetal", roughnessMetal);
				material->applyMaterial();

				CTransformEuler* t = sphereObj->getTransformEuler();
				t->setPosition(offset + core::vector3df(i * distance, 0.0f, j * distance));
				t->setScale(core::vector3df(0.5f, 0.5f, 0.5f));

				m_spheres.push_back(sphereObj);
			}
		}

		createCanvasText(zone, "PBR Shader", offset + core::vector3df(2.0f, 2.0f, 2.0f));
	}
#endif

#ifdef TEST_SG
	{
		// SpecGloss spheres
		int n = 5;
		core::vector3df offset(-12.0f, 0.0f, -n / 2.0f);
		float distance = 1.0f;

		for (int i = 0; i <= n; i++)
		{
			for (int j = 0; j <= n; j++)
			{
				CGameObject* sphereObj = zone->createEmptyObject();

				CSphere* sphere = sphereObj->addComponent<CSphere>();

				CMaterial* material = sphere->getMaterial();
				material->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGColor.xml");

				float specGloss[2];
				specGloss[0] = (float)i / (float)n;
				specGloss[1] = (float)j / (float)n;
				material->setUniform2("uSpecGloss", specGloss);
				material->setUniform4("uColor", SColor(255, 150, 150, 150));
				material->applyMaterial();

				CTransformEuler* t = sphereObj->getTransformEuler();
				t->setPosition(offset + core::vector3df(i * distance, 0.0f, j * distance));
				t->setScale(core::vector3df(0.5f, 0.5f, 0.5f));

				m_spheres.push_back(sphereObj);
			}
		}

		createCanvasText(zone, "SpecGloss Shader", offset + core::vector3df(2.0f, 2.0f, 2.0f));
	}
#endif

#ifdef TEST_MOBILE_SG
	{
		// SpecGloss spheres
		int n = 5;
		core::vector3df offset(-24.0f, 0.0f, -n / 2.0f);
		float distance = 1.0f;

		for (int i = 0; i <= n; i++)
		{
			for (int j = 0; j <= n; j++)
			{
				CGameObject* sphereObj = zone->createEmptyObject();

				CSphere* sphere = sphereObj->addComponent<CSphere>();

				CMaterial* material = sphere->getMaterial();
				material->changeShader("BuiltIn/Shader/Mobile/MobileSGColor.xml");

				float specGloss[2];
				specGloss[0] = (float)i / (float)n;
				specGloss[1] = (float)j / (float)n;
				material->setUniform2("uSpecGloss", specGloss);
				material->setUniform4("uColor", SColor(255, 150, 150, 150));
				material->applyMaterial();

				CTransformEuler* t = sphereObj->getTransformEuler();
				t->setPosition(offset + core::vector3df(i * distance, 0.0f, j * distance));
				t->setScale(core::vector3df(0.5f, 0.5f, 0.5f));

				m_spheres.push_back(sphereObj);
			}
		}

		createCanvasText(zone, "MobileSG Shader", offset + core::vector3df(2.0f, 2.0f, 2.0f));
	}
#endif

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, Transform::Oy);

	// rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initShadowForwarderPipeline(w, h);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);

	if (context->getPostProcessorPipeline())
		context->getPostProcessorPipeline()->enableAutoExposure(true);
}

void CViewInit::createCanvasText(CZone* zone, const char* text, const core::vector3df& position)
{
	CGameObject* canvasObject = zone->createEmptyObject();
	CCanvas* canvas = canvasObject->addComponent<CCanvas>();
	CGUIText* guiText = canvas->createText(core::rectf(0.0f, 0.0f, 700.0f, 100.0f), m_largeFont);
	guiText->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	guiText->setText(text);
	guiText->setPosition(core::vector3df(-350.0f, 0.0f, 0.0f));

	CGUIElement* rootGUI = canvas->getRootElement();
	rootGUI->setScale(core::vector3df(-0.004f, -0.004f, 0.004f));
	rootGUI->setPosition(position);

	canvas->setEnable3DBillboard(true);

	// set mask only for camera3d
	// culling bit: 10
	canvasObject->setCullingLayer(2);
}

void CViewInit::onDestroy()
{
	m_guiObject->remove();
	m_font->dropFont();
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
		CCamera* camera = context->getActiveCamera();
		CZone* zone = scene->getZone(0);

		if (m_bakeSHLighting == true)
		{
			m_bakeSHLighting = false;

			m_helmet->setVisible(false);
			for (CGameObject* obj : m_spheres)
				obj->setVisible(false);

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

			m_helmet->setVisible(true);
			for (CGameObject* obj : m_spheres)
				obj->setVisible(true);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
