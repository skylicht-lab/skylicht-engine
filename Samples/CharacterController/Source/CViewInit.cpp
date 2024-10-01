#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"
#include "Transform/CWorldTransformSystem.h"

#include "Primitive/CPlane.h"
#include "Primitive/CCube.h"
#include "Primitive/CSphere.h"
#include "SkySun/CSkySun.h"

#include "CapsuleMesh/CCapsuleComponent.h"
#include "LightProbes/CLightProbeRender.h"

#include "PhysicsEngine/CPhysicsEngine.h"
#include "Collider/CBvhMeshCollider.h"
#include "Collider/CBoxCollider.h"
#include "Collider/CCapsuleCollider.h"
#include "RigidBody/CRigidbody.h"
#include "CharacterController/CCharacterController.h"

#include "DirectionalInput/CDirectionalInput.h"
#include "Camera/C3rdCamera.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true)
{
	// CLightProbeRender::showProbe(true);
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

	// init physics engine
	Physics::CPhysicsEngine::getInstance()->initPhysics();

	// camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	C3rdCamera* followCamera = camObj->addComponent<C3rdCamera>();

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

	CMeshManager* meshManager = CMeshManager::getInstance();

	CEntityPrefab* prefab = meshManager->loadModel("SampleModels/TestScene/TestScene.dae", NULL, true);
	if (prefab != NULL)
	{
		// load material
		std::vector<std::string> textureFolders;
		ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial("SampleModels/TestScene/TestScene.mat", true, textureFolders);

		// create render mesh object
		CGameObject* testScene = zone->createEmptyObject();
		testScene->setStatic(true);

		// render mesh & init material
		CRenderMesh* renderer = testScene->addComponent<CRenderMesh>();
		renderer->initFromPrefab(prefab);
		renderer->initMaterial(materials);

		// physics & collider
		Physics::CBvhMeshCollider* collider = testScene->addComponent<Physics::CBvhMeshCollider>();
		collider->setMeshSource("SampleModels/TestScene/TestScene.dae");

		Physics::CRigidbody* rigidBody = testScene->addComponent<Physics::CRigidbody>();
		rigidBody->setDynamic(false);
		rigidBody->initRigidbody();

		CIndirectLighting* indirectLighting = testScene->addComponent<CIndirectLighting>();
	}

	Physics::CRigidbody* body;

	// Capsule
	float capsuleRadius = 0.5f;
	float capsuleHeight = 1.2f;

	CGameObject* capsuleObj = zone->createEmptyObject();

	// renderer
	CCapsuleComponent* capsule = capsuleObj->addComponent<CCapsuleComponent>();
	capsule->init(capsuleRadius, capsuleHeight);
	CMaterial* capsuleMaterial = capsule->getMaterial();
	capsuleMaterial->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
	capsuleMaterial->setUniform4("uColor", SColor(255, 200, 200, 200));
	capsuleMaterial->updateShaderParams();
	capsuleObj->addComponent<CIndirectLighting>();

	// collider & chracter
	Physics::CCapsuleCollider* capsuleCollider = capsuleObj->addComponent<Physics::CCapsuleCollider>();
	capsuleCollider->setCapsule(capsuleRadius, capsuleHeight);
	Physics::CCharacterController* characterController = capsuleObj->addComponent<Physics::CCharacterController>();
	characterController->initCharacter(capsuleRadius);
	characterController->setPosition(core::vector3df(0.0f, 5.0f, 0.0f));
	characterController->setRotation(core::vector3df(0.0f, 0.0f, 0.0f));
	characterController->syncTransform();

	// input to control capsule
	capsuleObj->addComponent<CDirectionalInput>();

	// follow camera
	followCamera->setFollowTarget(capsuleObj);
	followCamera->setTargetDistance(5.0f);

	m_objects.push_back(capsuleObj);

	// Cube
	CGameObject* cubeObj = zone->createEmptyObject();

	// renderer
	CCube* cube = cubeObj->addComponent<CCube>();
	CMaterial* material = cube->getMaterial();
	material->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

	// indirect lighting
	cubeObj->addComponent<CIndirectLighting>();

	// collider & rigidbody
	cubeObj->addComponent<Physics::CBoxCollider>();
	body = cubeObj->addComponent<Physics::CRigidbody>();
	body->initRigidbody();
	// body->setState(Physics::CRigidbody::Alway);
	body->setPosition(core::vector3df(-6.0f, 5.0f, 0.0f));
	body->setRotation(core::vector3df(45.0f, 45.0f, 0.0f));
	body->syncTransform();

	m_objects.push_back(cubeObj);

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
		listBundles.push_back("SampleModels.zip");
		listBundles.push_back("SampleModelsResource.zip");
		listBundles.push_back(getApplication()->getTexturePackageName("SampleModels.zip"));

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

			for (CGameObject* obj : m_objects)
				obj->setVisible(false);

			// light probe
			CGameObject* lightProbeObj = zone->createEmptyObject();
			CLightProbe* lightProbe = lightProbeObj->addComponent<CLightProbe>();
			lightProbeObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 5.0f, 0.0f));

			CGameObject* bakeCameraObj = scene->getZone(0)->createEmptyObject();
			CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
			scene->updateAddRemoveObject();

			// bake light probe
			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);

			std::vector<CLightProbe*> probes;
			probes.push_back(lightProbe);

			lm->bakeProbes(probes, bakeCamera, rp, scene->getEntityManager());

			for (CGameObject* obj : m_objects)
				obj->setVisible(true);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
