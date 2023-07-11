#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "SkyDome/CSkyDome.h"
#include "Lightmap/CLightmap.h"

#define LIGHTMAP_SPONZA

CViewInit::CViewInit()
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
	// init application
	CBaseApp* app = getApplication();

	// show console
	app->showDebugConsole();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Common.zip"), false, false);

	app->getFileSystem()->addFileArchive(app->getBuiltInPath("SampleModels.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("SampleModelsResource.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath(app->getTexturePackageName("SampleModels").c_str()), false, false);

	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Sponza.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("SponzaResource.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath(app->getTexturePackageName("Sponza").c_str()), false, false);

	// load basic shader
	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->initSGDeferredShader();

	// load font
	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// create a Scene
	CContext* context = CContext::getInstance();
	CScene* scene = context->initScene();

	// create a Zone in Scene
	CZone* zone = scene->createZone();

	// camera 2D
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// camera 3D
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	CCamera* camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(-10.0f, 5.0f, 10.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// sky
#ifdef LIGHTMAP_SPONZA
	ITexture* skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/Helipad.png");
#else
	ITexture* skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
#endif

	if (skyDomeTexture != NULL)
	{
		CSkyDome* skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

#ifdef LIGHTMAP_SPONZA
	// lighting
	CGameObject* lightObj = zone->createEmptyObject();

	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();

	// use direction light for specular
	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(2.0f, -5.0f, 1.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// Load model from Sponza.smesh
	// How to export "Sponza.smesh" see SampleLightmapUV
	CEntityPrefab* model = CMeshManager::getInstance()->loadModel("Sponza/Sponza.smesh", NULL, true);
#else
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	directionalLight->setBounce(2);

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	CEntityPrefab* model = CMeshManager::getInstance()->loadModel("SampleModels/Gazebo/gazebo.smesh", "LightmapUV");
#endif	

	if (model != NULL)
	{
		CGameObject* modelObject = zone->createEmptyObject();
		modelObject->setStatic(true);

		CRenderMesh* renderMesh = modelObject->addComponent<CRenderMesh>();
		renderMesh->initFromPrefab(model);

#ifdef LIGHTMAP_SPONZA
		std::vector<std::string> textureFolders;
		textureFolders.push_back("Sponza/Textures");

		// load material
		ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial("Sponza/Sponza.mat", true, textureFolders);

		// indirect lighting
		CIndirectLighting* indirect = modelObject->addComponent<CIndirectLighting>();

		std::vector<std::string> listTextures;

		// see SampleLightmapping
		listTextures.push_back("Sponza/LightMapRasterize_bounce_3_0.png");
		listTextures.push_back("Sponza/LightMapRasterize_bounce_3_1.png");
		listTextures.push_back("Sponza/LightMapRasterize_bounce_3_2.png");

		// see SampleLightmappingDirectional
		listTextures.push_back("Sponza/LightMap_0.png");
		listTextures.push_back("Sponza/LightMap_1.png");
		listTextures.push_back("Sponza/LightMap_2.png");

		ITexture* lightmapTexture = CTextureManager::getInstance()->getTextureArray(listTextures);
		indirect->setIndirectLightmap(lightmapTexture);
		indirect->setIndirectLightingType(CIndirectLighting::LightmapArray);

		// direction lightmap
		CLightmap* lightmap = modelObject->addComponent<CLightmap>();
		lightmap->setLightmap(lightmapTexture, 3);
#else
		// init default material
		ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(model);
		for (CMaterial* material : materials)
		{
			material->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

			float c[] = { 165.0f / 255.0f, 161.0f / 255.0f, 147 / 255.0f, 1.0f };
			material->setUniform4("uColor", c);
		}

		// indirect lighting
		CIndirectLighting* indirect = modelObject->addComponent<CIndirectLighting>();
		std::vector<std::string> listTextures;

		// see SampleLightmapping
		listTextures.push_back("SampleModels/Gazebo/LightMapRasterize_bounce_2_0.png");

		// see SampleLightmappingDirectional
		listTextures.push_back("SampleModels/Gazebo/LightMapDirectional_0.png");

		ITexture* lightmapTexture = CTextureManager::getInstance()->getTextureArray(listTextures);
		indirect->setIndirectLightmap(lightmapTexture);
		indirect->setIndirectLightingType(CIndirectLighting::LightmapArray);

		// direction lightmap
		CLightmap* lightmap = modelObject->addComponent<CLightmap>();
		lightmap->setLightmap(lightmapTexture, 1);
#endif

		renderMesh->initMaterial(materials);
	}

	// save to context	
	context->initLightmapRenderPipeline(app->getWidth(), app->getHeight(), true);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);
}

void CViewInit::initScene()
{

}

void CViewInit::onDestroy()
{

}

void CViewInit::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
}

void CViewInit::onRender()
{

}