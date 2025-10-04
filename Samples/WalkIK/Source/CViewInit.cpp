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

#define USE_SCORPION_MODEL

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
#ifdef USE_SCORPION_MODEL
	thirdCamera->setTargetDistance(8.0f);
#else
	thirdCamera->setTargetDistance(6.0f);
#endif
	thirdCamera->setOrientation(-135.0f, -45.0f);

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

	std::vector<std::string> leftLegs;
	std::vector<std::string> rightLegs;
	std::vector<CLeg*> llegs;
	std::vector<CLeg*> rlegs;

#ifdef USE_SCORPION_MODEL
	// custom for scorpion
	legController->setTargetDistance(3.0f);
	legController->setFootStepLength(0.4f);
	legController->setStepHeight(0.3f);
	legController->setMoveStepDistance(0.15f);

	CWorldTransformData* root = renderMesh->getChildTransform("boss_robot_01.fbx");
	leftLegs.push_back("L_leg_A_01");
	leftLegs.push_back("L_leg_B_01");
	leftLegs.push_back("L_leg_C_01");
	leftLegs.push_back("L_leg_D_01");

	rightLegs.push_back("R_leg_A_01");
	rightLegs.push_back("R_leg_B_01");
	rightLegs.push_back("R_leg_C_01");
	rightLegs.push_back("R_leg_D_01");
#else
	CWorldTransformData* root = renderMesh->getChildTransform("robot_01.fbx");
	leftLegs.push_back("leg_A_01");
	leftLegs.push_back("leg_B_01");
	rightLegs.push_back("leg_D_01");
	rightLegs.push_back("leg_C_01");
#endif

	int nLeg = (int)leftLegs.size();
	for (int i = 0; i < nLeg; i++)
	{
		legTransform = renderMesh->getChildTransform(leftLegs[i].c_str());
		if (legTransform)
			llegs.push_back(legController->addLeg(root, legTransform));

		legTransform = renderMesh->getChildTransform(rightLegs[i].c_str());
		if (legTransform)
			rlegs.push_back(legController->addLeg(root, legTransform));

#ifdef USE_SCORPION_MODEL
		// custom targetVector for scorpion
		core::vector3df targetVec;
		targetVec = llegs[i]->getTargetVector();
		targetVec.Z = targetVec.Z * 0.7f;
		llegs[i]->setTargetVector(targetVec);

		targetVec = rlegs[i]->getTargetVector();
		targetVec.Z = targetVec.Z * 0.7f;
		rlegs[i]->setTargetVector(targetVec);
#endif

		// link near legs to fix for nice walk cycle
		llegs[i]->addLink(rlegs[i]);
		if (i >= 1)
		{
			llegs[i]->addLink(llegs[i - 1]);
			rlegs[i]->addLink(rlegs[i - 1]);
		}
		if (i >= 2)
		{
			llegs[i]->addLink(llegs[i - 2]);
			rlegs[i]->addLink(rlegs[i - 2]);
		}
	}

	legController->resetFootPosition();

#ifdef USE_SCORPION_MODEL
	for (CLeg* leg : rlegs)
		leg->setFlip(true);
#endif

	// player input controller
	m_robot->addComponent<CPlayerController>();
}

CRenderMesh* CViewInit::initRobotRenderer(CGameObject* obj)
{
	// render mesh & init material
	std::vector<std::string> searchTextureFolders;

#ifdef USE_SCORPION_MODEL
	const char* model = "SampleModels/Robot/boss_robot_01.fbx";
#else
	const char* model = "SampleModels/Robot/robot_01.fbx";
#endif

	CEntityPrefab* modelPrefab = CMeshManager::getInstance()->loadModel(model, NULL, true);

	CRenderMesh* renderer = obj->addComponent<CRenderMesh>();
	renderer->initFromPrefab(modelPrefab);

	CTextureManager* textureManager = CTextureManager::getInstance();

	ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(modelPrefab);
	materials[0]->changeShader("BuiltIn/Shader/PBR/Forward/PBRSkinNoEmissive.xml");

#ifdef USE_SCORPION_MODEL
	ITexture* albedoMap = textureManager->getTexture("SampleModels/Robot/scorpion_a.png");
	ITexture* normalMap = textureManager->getTexture("BuiltIn/Textures/NullNormalMap.png");
	ITexture* rmaMap = textureManager->getTexture("SampleModels/Robot/scorpion_rma.png");
#else
	ITexture* albedoMap = textureManager->getTexture("SampleModels/Robot/robot_01_a.png");
	ITexture* normalMap = textureManager->getTexture("BuiltIn/Textures/NullNormalMap.png");
	ITexture* rmaMap = textureManager->getTexture("SampleModels/Robot/robot_01_rma.png");
#endif

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

	std::vector<std::string> leftLegs;
	std::vector<std::string> rightLegs;
	std::vector<CLegIK*> llegs;
	std::vector<CLegIK*> rlegs;

#ifdef USE_SCORPION_MODEL
	CWorldTransformData* root = renderMesh->getChildTransform("boss_robot_01.fbx");
	leftLegs.push_back("L_leg_A_01");
	leftLegs.push_back("L_leg_B_01");
	leftLegs.push_back("L_leg_C_01");
	leftLegs.push_back("L_leg_D_01");

	rightLegs.push_back("R_leg_A_01");
	rightLegs.push_back("R_leg_B_01");
	rightLegs.push_back("R_leg_C_01");
	rightLegs.push_back("R_leg_D_01");
#else
	CWorldTransformData* root = renderMesh->getChildTransform("robot_01.fbx");
	leftLegs.push_back("leg_A_01");
	leftLegs.push_back("leg_B_01");

	rightLegs.push_back("leg_C_01");
	rightLegs.push_back("leg_D_01");
#endif

	int nLeg = (int)leftLegs.size();
	for (int i = 0; i < nLeg; i++)
	{
		legTransform = renderMesh->getChildTransform(leftLegs[i].c_str());
		if (legTransform)
			llegs.push_back(legReplayer->addLeg(root, legTransform));

		legTransform = renderMesh->getChildTransform(rightLegs[i].c_str());
		if (legTransform)
			rlegs.push_back(legReplayer->addLeg(root, legTransform));
	}

#ifdef USE_SCORPION_MODEL
	for (CLegIK* leg : rlegs)
		leg->setFlip(true);
#endif

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
