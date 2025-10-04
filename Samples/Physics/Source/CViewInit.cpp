#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "Primitive/CPlane.h"
#include "Primitive/CCube.h"
#include "Primitive/CSphere.h"

#include "SkyDome/CSkyDome.h"

#include "PhysicsEngine/CPhysicsEngine.h"
#include "Collider/CStaticPlaneCollider.h"
#include "Collider/CBoxCollider.h"
#include "Collider/CSphereCollider.h"
#include "RigidBody/CRigidbody.h"

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

	// init physics engine
	Physics::CPhysicsEngine::getInstance()->initPhysics();

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

	// plane
	CContainerObject* planePhysics = zone->createContainerObject();
	planePhysics->setName("Plane");

	planePhysics->addComponent<Physics::CStaticPlaneCollider>();
	Physics::CRigidbody* body = planePhysics->addComponent<Physics::CRigidbody>();
	body->setDynamic(false); // this is kinematic
	body->initRigidbody();

	CGameObject* planeObject = planePhysics->createEmptyObject();
	CPlane* plane = planeObject->addComponent<CPlane>();
	plane->getMaterial()->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetersGrid.xml");

	// indirect lighting
	planeObject->addComponent<CIndirectLighting>();

	// scale plane for render
	core::matrix4 m;
	m.setScale(core::vector3df(100.0, 1.0f, 100.0f));
	planeObject->getTransform()->setRelativeTransform(m);


	// Cube 1
	CGameObject* cubeObj = zone->createEmptyObject();
	cubeObj->setName("Cube 1");

	// cube & material
	CCube* cube = cubeObj->addComponent<CCube>();
	CMaterial* material = cube->getMaterial();
	material->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

	// indirect lighting
	cubeObj->addComponent<CIndirectLighting>();

	// Init physics
	cubeObj->addComponent<Physics::CBoxCollider>();
	body = cubeObj->addComponent<Physics::CRigidbody>();
	body->initRigidbody();
	body->setPosition(core::vector3df(0.0f, 5.0f, 0.0f));
	body->setRotation(core::vector3df(45.0f, 45.0f, 0.0f));
	body->syncTransform();
	body->OnCollision = [](Physics::ICollisionObject* a, Physics::ICollisionObject* b, Physics::SCollisionContactPoint* colliderInfo, int numContact)
		{
			Physics::CRigidbody* bodyA = dynamic_cast<Physics::CRigidbody*>(a);
			Physics::CRigidbody* bodyB = dynamic_cast<Physics::CRigidbody*>(b);

			if (bodyA == NULL || bodyB == NULL)
				return;

			if (bodyA->getState() != Physics::CRigidbody::Sleep ||
				bodyB->getState() != Physics::CRigidbody::Sleep)
			{
				char log[1024];
				sprintf(log, "[%s - %s] collision [%s - %s]",
					bodyA->getGameObject()->getNameA(),
					bodyA->getStateName(),
					bodyB->getGameObject()->getNameA(),
					bodyB->getStateName());
				os::Printer::log(log);
			}
		};

	// Cube 2
	cubeObj = zone->createEmptyObject();
	cubeObj->setName("Cube 2");

	// cube & material
	cube = cubeObj->addComponent<CCube>();
	material = cube->getMaterial();
	material->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

	// indirect lighting
	cubeObj->addComponent<CIndirectLighting>();

	cubeObj->addComponent<Physics::CBoxCollider>();
	body = cubeObj->addComponent<Physics::CRigidbody>();
	body->initRigidbody();
	body->setPosition(core::vector3df(0.0f, 10.0f, 0.0f));
	body->syncTransform();
	body->OnCollision = [](Physics::ICollisionObject* a, Physics::ICollisionObject* b, Physics::SCollisionContactPoint* colliderInfo, int numContact)
		{
			Physics::CRigidbody* bodyA = dynamic_cast<Physics::CRigidbody*>(a);
			Physics::CRigidbody* bodyB = dynamic_cast<Physics::CRigidbody*>(b);

			if (bodyA == NULL || bodyB == NULL)
				return;

			if (bodyA->getState() != Physics::CRigidbody::Sleep ||
				bodyB->getState() != Physics::CRigidbody::Sleep)
			{
				char log[1024];
				sprintf(log, "[%s - %s] collision [%s - %s]",
					bodyA->getGameObject()->getNameA(),
					bodyA->getStateName(),
					bodyB->getGameObject()->getNameA(),
					bodyB->getStateName());
				os::Printer::log(log);
			}
		};

	// Sphere
	CGameObject* sphereObj = zone->createEmptyObject();
	sphereObj->setName("Sphere");

	// cube & material
	CSphere* sphere = sphereObj->addComponent<CSphere>();
	material = sphere->getMaterial();
	material->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

	// indirect lighting
	sphereObj->addComponent<CIndirectLighting>();
	sphereObj->addComponent<Physics::CSphereCollider>();
	body = sphereObj->addComponent<Physics::CRigidbody>();
	body->setRollingFriction(0.02f);
	body->setSpinningFriction(0.02f);
	body->initRigidbody();
	body->setPosition(core::vector3df(1.0f, 5.0f, 0.0f));
	body->syncTransform();


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
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
