#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "LegController/CLegController.h"
#include "Record/CLegReplayer.h"

#include "CPlayerController.h"

#include "Primitive/CPlane.h"
#include "Primitive/CCube.h"
#include "SkyDome/CSkyDome.h"
#include "SkySun/CSkySun.h"

#include "LightProbes/CLightProbeRender.h"
#include "ReflectionProbe/CReflectionProbeRender.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true)
{
	// CLightProbeRender::showProbe(true);
	// CReflectionProbeRender::showProbe(true);
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
	shaderMgr->initSGDeferredShader();
	shaderMgr->initPBRForwarderShader();

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

	// crate gui camera
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
	C3rdCamera* thirdCamera = camObj->addComponent<C3rdCamera>();

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
	reflectionProbeObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 2.0f, 0.0f));
	m_reflectionProbe = reflectionProbeObj->addComponent<CReflectionProbe>();

	// Plane
	m_plane = zone->createEmptyObject();
	m_plane->setName("Plane");

	CPlane* plane = m_plane->addComponent<CPlane>();
	plane->getMaterial()->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetersGrid.xml");

	// indirect lighting
	m_plane->addComponent<CIndirectLighting>();

	// scale plane for render
	core::matrix4 m;
	m.setScale(core::vector3df(100.0, 1.0f, 100.0f));
	m_plane->getTransform()->setRelativeTransform(m);

	// robot, that player control by keyboard
	initRobot();

	// robot, that replay the animation
	initReplayRobot();

	// robot, that play .sanim that exported from recorded
	// initAnimRobot();

	// setting camera following robot
	thirdCamera->setFollowTarget(m_robot);
	thirdCamera->setTargetDistance(6.0f);
	thirdCamera->setOrientation(-135.0f, -45.0f);

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	directionalLight->setIntensity(1.2f);

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, Transform::Oy);

	// rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initRenderPipeline(w, h);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);

	context->getPostProcessorPipeline()->enableAutoExposure(false);
	context->getPostProcessorPipeline()->enableBloomEffect(false);
}

void CViewInit::initRobot()
{
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	m_robot = zone->createEmptyObject();
	m_robot->setName("robot");

	CRenderMesh* renderMesh = initRobotRenderer(m_robot);

	// leg controller
	CLegController* legController = m_robot->addComponent<CLegController>();
	CWorldTransformData* legTransform = NULL;

	// need update a frame
	scene->getEntityManager()->update();

	CWorldTransformData* root = renderMesh->getChildTransform("robot_01.fbx");

	CLeg* legs[4];

	// add leg
	legTransform = renderMesh->getChildTransform("leg_A_01");
	if (legTransform)
		legs[0] = legController->addLeg(root, legTransform);
	legTransform = renderMesh->getChildTransform("leg_B_01");
	if (legTransform)
		legs[1] = legController->addLeg(root, legTransform);
	legTransform = renderMesh->getChildTransform("leg_C_01");
	if (legTransform)
		legs[2] = legController->addLeg(root, legTransform);
	legTransform = renderMesh->getChildTransform("leg_D_01");
	if (legTransform)
		legs[3] = legController->addLeg(root, legTransform);

	// link near legs to fix for nice walk cycle
	legs[0]->addLink(legs[3]);
	legs[1]->addLink(legs[2]);
	legs[0]->addLink(legs[1]);
	legs[2]->addLink(legs[3]);

	// player input controller
	m_robot->addComponent<CPlayerController>();
}

CRenderMesh* CViewInit::initRobotRenderer(CGameObject* obj)
{
	// render mesh & init material
	std::vector<std::string> searchTextureFolders;
	CEntityPrefab* modelPrefab = CMeshManager::getInstance()->loadModel("SampleModels/Robot/robot_01.fbx", NULL, true);

	CRenderMesh* renderer = obj->addComponent<CRenderMesh>();
	renderer->initFromPrefab(modelPrefab);

	CTextureManager* textureManager = CTextureManager::getInstance();

	ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(modelPrefab);
	materials[0]->changeShader("BuiltIn/Shader/PBR/Forward/PBRSkin.xml");
	ITexture* albedoMap = textureManager->getTexture("SampleModels/Robot/robot_01_a.png");
	ITexture* normalMap = textureManager->getTexture("BuiltIn/Textures/NullNormalMap.png");
	ITexture* rmaMap = textureManager->getTexture("SampleModels/Robot/robot_01_rma.png");
	materials[0]->setUniformTexture("uTexAlbedo", albedoMap);
	materials[0]->setUniformTexture("uTexNormal", normalMap);
	materials[0]->setUniformTexture("uTexRMA", rmaMap);

	renderer->initMaterial(materials);
	renderer->printEntites();

	// indirect lighting
	obj->addComponent<CIndirectLighting>();

	return renderer;
}

void CViewInit::initReplayRobot()
{
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	CGameObject* robot = zone->createEmptyObject();
	robot->setName("robot-replay");

	CRenderMesh* renderMesh = initRobotRenderer(robot);

	// leg replayer
	CLegReplayer* legReplayer = robot->addComponent<CLegReplayer>();
	CWorldTransformData* legTransform = NULL;

	// need update a frame
	scene->getEntityManager()->update();

	CWorldTransformData* root = renderMesh->getChildTransform("robot_01.fbx");
	CLegIK* legs[4];

	// add leg
	legTransform = renderMesh->getChildTransform("leg_A_01");
	if (legTransform)
		legs[0] = legReplayer->addLeg(root, legTransform);
	legTransform = renderMesh->getChildTransform("leg_B_01");
	if (legTransform)
		legs[1] = legReplayer->addLeg(root, legTransform);
	legTransform = renderMesh->getChildTransform("leg_C_01");
	if (legTransform)
		legs[2] = legReplayer->addLeg(root, legTransform);
	legTransform = renderMesh->getChildTransform("leg_D_01");
	if (legTransform)
		legs[3] = legReplayer->addLeg(root, legTransform);

	// hide this robot
	robot->setVisible(false);
}

void CViewInit::initAnimRobot()
{
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	CGameObject* robot = zone->createEmptyObject();
	robot->setName("robot-anim");

	CRenderMesh* renderMesh = initRobotRenderer(robot);

	CAnimationController* animController = robot->addComponent<CAnimationController>();
	CSkeleton* skeleton = animController->createSkeleton();

	CAnimationClip* clip = CAnimationManager::getInstance()->loadAnimation("walk.sanim");
	skeleton->setAnimation(clip, true);

	robot->getTransformEuler()->setPosition(core::vector3df(2.0f, 0.0f, 2.0f));
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

		if (m_bakeSHLighting == true)
		{
			m_bakeSHLighting = false;

			CZone* zone = scene->getZone(0);

			m_plane->setVisible(false);
			m_robot->setVisible(false);

			// light probe
			CGameObject* lightProbeObj = zone->createEmptyObject();
			CLightProbe* lightProbe = lightProbeObj->addComponent<CLightProbe>();
			lightProbeObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

			CGameObject* bakeCameraObj = scene->getZone(0)->createEmptyObject();
			CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
			scene->updateAddRemoveObject();
			scene->update();

			// bake light probe
			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);

			std::vector<CLightProbe*> probes;
			probes.push_back(lightProbe);

			lm->bakeProbes(probes, bakeCamera, rp, scene->getEntityManager());

			// bake environment
			m_reflectionProbe->bakeProbe(bakeCamera, rp, scene->getEntityManager());

			m_plane->setVisible(true);
			m_robot->setVisible(true);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
