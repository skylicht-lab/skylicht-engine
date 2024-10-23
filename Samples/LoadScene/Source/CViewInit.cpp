#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"
#include "Transform/CWorldTransformSystem.h"

#include "Scene/CSceneImporter.h"

#include "PhysicsEngine/CPhysicsEngine.h"
using namespace Physics;

#include "IndirectLighting/CIndirectLighting.h"
#include "LightProbes/CLightProbes.h"
#include "Primitive/CCube.h"
#include "Primitive/CPlane.h"
#include "SkySun/CSkySun.h"
#include "Collider/CBoxCollider.h"
#include "Collider/CStaticPlaneCollider.h"
#include "Collider/CCapsuleCollider.h"
#include "RigidBody/CRigidbody.h"

#include "Primitive/CCapsule.h"
#include "CharacterController/CCharacterController.h"
#include "DirectionalInput/CDirectionalInput.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true),
	m_loadScene(NULL),
	m_guiObject(NULL),
	m_textInfo(NULL),
	m_font(NULL)
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

	CScene* scene = m_loadScene;
	CZone* zone = scene->getZone(0);


	// init player controller
	float capsuleRadius = 0.5f;
	float capsuleHeight = 1.2f;

	CGameObject* capsuleObj = zone->createEmptyObject();

	// renderer
	CCapsule* capsule = capsuleObj->addComponent<CCapsule>();
	capsule->setRadius(capsuleRadius);
	capsule->setHeight(capsuleHeight);
	capsule->init();

	// indirect lighting
	capsuleObj->addComponent<CIndirectLighting>();

	// collider & character
	Physics::CCapsuleCollider* capsuleCollider = capsuleObj->addComponent<Physics::CCapsuleCollider>();
	capsuleCollider->setCapsule(capsuleRadius, capsuleHeight);
	CCharacterController* m_player = capsuleObj->addComponent<Physics::CCharacterController>();
	m_player->initCharacter(capsuleRadius);
	m_player->setPosition(core::vector3df(0.0f, 5.0f, 0.0f));
	m_player->setRotation(core::vector3df(0.0f, 0.0f, 0.0f));
	m_player->syncTransform();

	// input to control capsule
	capsuleObj->addComponent<CDirectionalInput>();

	// follow camera
	CGameObject* cameraObj = zone->createEmptyObject();
	CCamera* camera = cameraObj->addComponent<CCamera>();
	C3rdCamera* followCamera = cameraObj->addComponent<C3rdCamera>();
	if (followCamera)
	{
		followCamera->setFollowTarget(capsuleObj);
		followCamera->setTargetDistance(5.0f);
	}

	// gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initRenderPipeline(w, h);

	context->releaseScene();
	context->initScene(scene);

	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
}

void CViewInit::onDestroy()
{
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
		listBundles.push_back("SampleScene.zip");
		listBundles.push_back("SampleSceneResource.zip");

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

		m_initState = CViewInit::LoadScene;
#endif
	}
	break;
	case CViewInit::LoadScene:
	{
		// init physics engine
		Physics::CPhysicsEngine* engine = Physics::CPhysicsEngine::getInstance();
		engine->initPhysics();
		// engine->enableDrawDebug(true, true);

		m_loadScene = new CScene();

		// declare component used in scene
		// see: SampleScene/Demo.txt
		USE_COMPONENT(CBoxCollider);
		USE_COMPONENT(CCube);
		USE_COMPONENT(CDirectionalLight);
		USE_COMPONENT(CIndirectLighting);
		USE_COMPONENT(CLightProbes);
		USE_COMPONENT(CPlane);
		USE_COMPONENT(CReflectionProbe);
		USE_COMPONENT(CRenderMesh);
		USE_COMPONENT(CRigidbody);
		USE_COMPONENT(CSkySun);
		USE_COMPONENT(CStaticPlaneCollider);
		USE_COMPONENT(CTransformEuler);

		// load scene
		CSceneImporter::beginImportScene(m_loadScene, "SampleScene/Demo.scene");
		m_initState = CViewInit::Loading;

		m_textInfo->setText("Loading");
	}
	break;
	case CViewInit::Loading:
	{
		if (CSceneImporter::updateLoadScene())
		{
			m_initState = CViewInit::InitScene;
		}

		char log[512];
		sprintf(log, "Loading: %d%%", (int)(CSceneImporter::getLoadingPercent() * 100.0f));
		os::Printer::log(log);
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

	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
