#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"
#include "ViewManager/CViewManager.h"
#include "TextureManager/CTextureManager.h"

#include "MeshManager/CMeshManager.h"
#include "Animation/CAnimationManager.h"
#include "Animation/CAnimationController.h"

#include "Context/CContext.h"

#include "Material/Shader/CShaderManager.h"
#include "Material/CMaterialManager.h"

#include "Camera/CEditorCamera.h"
#include "Lighting/CDirectionalLight.h"
#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"
#include "RenderMesh/CRenderMesh.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char *name)
{
#ifdef __EMSCRIPTEN__
	// path from ./PrjEmscripten/Projects/MainApp
	std::string assetPath = std::string("../../../Bin/BuiltIn/") + std::string(name);
	return io::path(assetPath.c_str());
#elif defined(WINDOWS_STORE)
	std::string assetPath = std::string("Assets\\") + std::string(name);
	return io::path(assetPath.c_str());
#else
	return io::path(name);
#endif
}

void CViewInit::onInit()
{
	getApplication()->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGLighting.xml");
}

void CViewInit::initScene()
{
	CBaseApp *app = getApplication();

	CScene *scene = CContext::getInstance()->initScene();
	CZone *zone = scene->createZone();

	// camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	CCamera *camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(2.0f, 1.0f, 2.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Demo/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	lightObj->addComponent<CDirectionalLight>();
	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -2.0f, -2.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// grid
	// zone->createEmptyObject()->addComponent<CGridPlane>();

	// sponza
	/*
	CMeshManager *meshManager = CMeshManager::getInstance();
	CEntityPrefab *prefab = NULL;

	std::vector<std::string> textureFolders;
	textureFolders.push_back("Demo/Sponza/Textures");

	// load model
	prefab = meshManager->loadModel("Demo/Sponza/Sponza.dae", NULL, true);
	if (prefab != NULL)
	{
		// export model material
		ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial("Demo/Sponza/Sponza.xml", true, textureFolders);
		for (CMaterial *&material : materials)
		{
			material->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
			material->autoDetectLoadTexture();
		}

		// create render mesh object
		CGameObject *sponza = zone->createEmptyObject();
		CRenderMesh *renderer = sponza->addComponent<CRenderMesh>();
		renderer->initFromPrefab(prefab);
		renderer->initMaterial(materials);
	}
	*/

	// test dae model & animation
	/*
	CAnimationManager *animManager = CAnimationManager::getInstance();
	CAnimationClip *animIdle = animManager->loadAnimation("Demo/Model3D/Hero@Idle.dae");
	CAnimationClip *animWalkForward = animManager->loadAnimation("Demo/Model3D/Hero@WalkForward.dae");
	CAnimationClip *animRunForward = animManager->loadAnimation("Demo/Model3D/Hero@RunForward.dae");

	prefab = meshManager->loadModel("Demo/Model3D/Hero.dae", "Demo/Model3D/Textures", false);
	if (prefab != NULL)
	{
		// instance object 1
		CGameObject *model = zone->createEmptyObject();
		model->addComponent<CRenderMesh>()->initFromPrefab(prefab);

		// setup animation
		CAnimationController *animController = model->addComponent<CAnimationController>();

		CSkeleton *skeletonIdle = animController->createSkeleton();
		CSkeleton *skeletonWalkForward = animController->createSkeleton();
		CSkeleton *skeletonRunForward = animController->createSkeleton();

		CSkeleton *output = animController->createSkeleton();
		output->setAnimationType(CSkeleton::Blending);

		// set animation clip
		skeletonIdle->setAnimation(animIdle, true);
		skeletonIdle->getTimeline().Weight = 0.0f;

		skeletonWalkForward->setAnimation(animWalkForward, true);
		skeletonWalkForward->getTimeline().Weight = 0.0f;

		skeletonRunForward->setAnimation(animRunForward, true);
		skeletonRunForward->getTimeline().Weight = 1.0f;

		// blending
		skeletonIdle->setTarget(output);
		skeletonWalkForward->setTarget(output);
		skeletonRunForward->setTarget(output);

		// output animation
		animController->setOutput(output);

		// setup transform
		CTransformEuler *transform = model->getTransformEuler();
		transform->setPosition(core::vector3df(0.0f, 0.0f, 2.0f));
		transform->setYaw(45.0f);
	}
	*/

	// save to context
	CContext *context = CContext::getInstance();
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
}

void CViewInit::onDestroy()
{

}

void CViewInit::onUpdate()
{
	CContext *context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();
#ifdef __EMSCRIPTEN__
		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL("Demo.zip", "Demo.zip");
			m_getFile->download(CGetFileURL::Get);
		}
		else
		{
			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				// add demo.zip after it downloaded
				fileSystem->addFileArchive("Demo.zip", false, false);
				m_initState = CViewInit::InitScene;
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
			}
		}
#else

#if defined(WINDOWS_STORE)
		fileSystem->addFileArchive(getBuiltInPath("Demo.zip"), false, false);
#else
		fileSystem->addFileArchive("Demo.zip", false, false);
#endif	

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
		CScene *scene = context->getScene();
		if (scene != NULL)
			scene->update();

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
	break;
	}
}

void CViewInit::onRender()
{

}