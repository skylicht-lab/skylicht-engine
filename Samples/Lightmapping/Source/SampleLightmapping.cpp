#include "pch.h"
#include "SkylichtEngine.h"
#include "Context/CContext.h"
#include "SampleLightmapping.h"

#include "SkyDome/CSkyDome.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmapping *app = new SampleLightmapping();
	getApplication()->registerAppEvent("SampleLightmapping", app);
}

SampleLightmapping::SampleLightmapping()
{
	CContext::createGetInstance();
}

SampleLightmapping::~SampleLightmapping()
{
	CContext::releaseInstance();
}

void SampleLightmapping::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Common.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("LightmapUV.zip"), false, false);

	// load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormal.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Specular.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Diffuse.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossinessMask.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLightShadow.xml");

	// create a Scene
	CContext *context = CContext::getInstance();
	CScene *scene = context->initScene();

	// create a Zone in Scene
	CZone *zone = scene->createZone();

	// camera 2D
	CGameObject *guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// camera 3D
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	CCamera *camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(-10.0f, 5.0f, 10.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
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

	// gazebo object
	CEntityPrefab *model = CMeshManager::getInstance()->loadModel("LightmapUV/gazebo.smesh", "LightmapUV");

	if (model != NULL)
	{
		CGameObject *gazeboObj = zone->createEmptyObject();
		CRenderMesh *renderMesh = gazeboObj->addComponent<CRenderMesh>();
		renderMesh->initFromPrefab(model);
	}

	// save to context	
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);
}

void SampleLightmapping::onUpdate()
{
	CContext *context = CContext::getInstance();

	// update direction light
	context->updateDirectionLight();

	// update application
	context->getScene()->update();
}

void SampleLightmapping::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CCamera *guiCamera = context->getGUICamera();

	CScene *scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->updateDirectionLight();

		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void SampleLightmapping::onPostRender()
{
	// post render application
}

bool SampleLightmapping::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleLightmapping::onResume()
{
	// resume application
}

void SampleLightmapping::onPause()
{
	// pause application
}

void SampleLightmapping::onQuitApp()
{
	// end application
	delete this;
}