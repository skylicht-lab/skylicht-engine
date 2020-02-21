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
#include "Lighting/CPointLight.h"
#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"
#include "RenderMesh/CRenderMesh.h"
#include "Graphics2D/CCanvas.h"

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
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormal.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Specular.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossinessMask.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLightShadow.xml");
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

	// gui camera
	CGameObject *guiCameraObj = zone->createEmptyObject();
	guiCameraObj->addComponent<CCamera>();
	CCamera *guiCamera = guiCameraObj->getComponent<CCamera>();

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Demo/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -7.0f, -1.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	core::vector3df pointLightPosition[] = {
		{-5.595442f, 1.2f, 2.00912f},
		{-5.6f, 1.2f, -2.25},
		{6.018463f, 1.2f, 2.0211f},
		{6.007851f, 1.2f, -2.237712f},

		{3.09f, 0.8f, -1.10477f},
		{3.09f, 0.8f, 0.71455f},
		{-2.4447f, 0.8f, -1.0884f},
		{-2.4447f, 0.8f,  0.7141f},
	};

	for (int i = 0; i < 8; i++)
	{
		CGameObject *pointLightObj = zone->createEmptyObject();

		CPointLight *pointLight = pointLightObj->addComponent<CPointLight>();
		pointLight->setShadow(true);

		if (i >= 4)
			pointLight->setRadius(1.5f);
		else
			pointLight->setRadius(3.0f);

		CTransformEuler *pointLightTransform = pointLightObj->getTransformEuler();
		pointLightTransform->setPosition(pointLightPosition[i]);
	}

	// sponza
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

		// save material
		// CMaterialManager::getInstance()->saveMaterial(materials, "../Assets/Demo/Sponza/Sponza.xml");

		// create render mesh object
		CGameObject *sponza = zone->createEmptyObject();
		CRenderMesh *renderer = sponza->addComponent<CRenderMesh>();
		renderer->initFromPrefab(prefab);
		renderer->initMaterial(materials);
	}

	// gui
	CGameObject *guiObject = zone->createEmptyObject();
	CCanvas *canvas = guiObject->addComponent<CCanvas>();
	CGUIImage *guiImage = canvas->createImage();

	// save to context
	CContext *context = CContext::getInstance();
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);
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