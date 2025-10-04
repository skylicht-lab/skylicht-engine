#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"
#include "Transform/CWorldTransformSystem.h"

#include "Primitive/CCube.h"
#include "SkySun/CSkySun.h"

#include "Boids/CBoidSystem.h"
#include "Boids/CBoidAnimationSystem.h"

#include "Material/Shader/Instancing/CTBNSGInstancing.h"

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
	directionalLight->setIntensity(1.2f);

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, Transform::Oy);


	// load toon instancing shader
	CShaderManager* shaderManager = CShaderManager::getInstance();
	shaderManager->loadShader("BuiltIn/Shader/Toon/ToonShadowInstancing.xml");
	shaderManager->loadShader("BuiltIn/Shader/Toon/ToonShadow.xml");

	float envMin = -25.0f;
	float envMax = 25.0f;

	// init environment
	initEnviroment(zone, envMin, envMax);

	// load animation
	CAnimationManager* animManager = CAnimationManager::getInstance();
	CAnimationClip* animIdle = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Idle.fbx");
	CAnimationClip* animWalk = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Walk.fbx");
	CAnimationClip* animRun = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Running.fbx");

	std::vector<CAnimationClip*> clips;
	clips.push_back(animIdle);
	clips.push_back(animWalk);
	clips.push_back(animRun);

	// init crowd
	int fps = 60;
	initCrowdByAnimTexture(zone, envMin, envMax, clips, fps);

	// init bold system
	CEntityManager* entityManager = scene->getEntityManager();

	// add bold system (that will update moving for CBoidData)
	float wallDepth = 0.5f;
	CBoidSystem* boldSystem = entityManager->addSystem<CBoidSystem>();
	boldSystem->setBounds(
		envMin + wallDepth, // min x
		envMax - wallDepth, // max x
		envMin + wallDepth, // min z
		envMax - wallDepth, // max z
		2.5f // distance near wall, that bold will begin turn
	);

	// that let CBoidSystem will update before CWorldTransformSystem
	int worldTransformOrder = entityManager->getSystem<CWorldTransformSystem>()->getSystemOrder();
	boldSystem->setSystemOrder(worldTransformOrder - 1);
	entityManager->notifySystemOrderChanged();

	// add bold system (that will update animation clip)
	CBoidAnimationSystem* animSystem = entityManager->addSystem<CBoidAnimationSystem>();
	animSystem->addClip(animIdle, 0, fps, 0.0f);
	animSystem->addClip(animWalk, 1, fps, 0.01f);
	animSystem->addClip(animRun, 2, fps, 0.03f);

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initShadowForwarderPipeline(w, h);
	context->setActiveZone(zone);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);

	bool use2Cascade = false;
	bool noCascade = false;

	if (use2Cascade)
	{
		// Test use 2 cascade shadow
		context->getShadowMapRenderPipeline()->setShadowCascade(2);
	}
	else if (noCascade)
	{
		// Test no shadow cascade (60m)
		context->getShadowMapRenderPipeline()->setNoShadowCascade(2048, 60.0f);
	}

	CPostProcessorRP* postProcess = context->getPostProcessorPipeline();
	postProcess->enableAutoExposure(false);
	postProcess->enableBloomEffect(false);
}

void CViewInit::initEnviroment(CZone* zone, float& envMin, float& envMax)
{
	int minBound = -2;
	int maxBound = 2;

	float space = 8.0f;
	float halfSpace = space * 0.5f;

	float scale = 0.16f;
	core::vector3df scaleVector(scale, scale, scale);

	envMin = minBound * space - halfSpace;
	envMax = maxBound * space + halfSpace;

	CMeshManager* meshManager = CMeshManager::getInstance();

	std::vector<std::string> searchTextureFolders;
	ArrayMaterial envMaterials = CMaterialManager::getInstance()->loadMaterial("SampleBoids/Environment/Environment.mat", true, searchTextureFolders);

	CEntityPrefab* groundPrefab = meshManager->loadModel("SampleBoids/Environment/1_Ground_1.fbx", NULL, true);
	if (groundPrefab)
	{
		CGameObject* plane = zone->createEmptyObject();
		CRenderMeshInstancing* meshInstancing = plane->addComponent<CRenderMeshInstancing>();
		meshInstancing->initFromPrefab(groundPrefab);
		meshInstancing->initMaterial(envMaterials);

		for (int i = minBound; i <= maxBound; i++)
		{
			for (int j = minBound; j <= maxBound; j++)
			{
				CEntity* entity = meshInstancing->spawn();

				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative.setTranslation(core::vector3df(i * space, 0.0f, j * space));
				transform->Relative.setScale(scaleVector);
			}
		}

		meshInstancing->applyShareTransformBuffer();
		meshInstancing->applyShareMaterialBuffer();

		plane->addComponent<CIndirectLighting>();

		m_allObjects.push_back(plane);
	}

	std::vector<std::string> randomWallMesh;
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_1.fbx");
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_2.fbx");
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_3.fbx");
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_4.fbx");

	std::vector<CRenderMeshInstancing*> meshInstancings;

	for (std::string& s : randomWallMesh)
	{
		CEntityPrefab* prefab = meshManager->loadModel(s.c_str(), NULL, true);
		if (prefab)
		{
			CGameObject* border = zone->createEmptyObject();
			CRenderMeshInstancing* meshInstancing = border->addComponent<CRenderMeshInstancing>();
			meshInstancing->initFromPrefab(prefab);
			meshInstancing->initMaterial(envMaterials);

			border->addComponent<CIndirectLighting>();

			meshInstancings.push_back(meshInstancing);

			m_allObjects.push_back(border);
		}
	}

	core::matrix4 rot180, rot90, rot270, scaleMat;
	rot90.setRotationDegrees(core::vector3df(0.0f, 90.0f, 0.0f));
	rot180.setRotationDegrees(core::vector3df(0.0f, 180.0f, 0.0f));
	rot270.setRotationDegrees(core::vector3df(0.0f, 270, 0.0f));
	scaleMat.setScale(scaleVector);

	{
		CEntity* entity;
		CWorldTransformData* transform;

		int random = 0;
		int count = (int)meshInstancings.size();
		if (count > 0)
		{
			for (int i = minBound; i <= maxBound; i++)
			{
				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative.setTranslation(core::vector3df(i * space, 0.0f, -maxBound * space - halfSpace));
				transform->Relative.setScale(scaleVector);

				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative = rot180 * scaleMat;
				transform->Relative.setTranslation(core::vector3df(i * space, 0.0f, maxBound * space + halfSpace));

				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative = rot90 * scaleMat;
				transform->Relative.setTranslation(core::vector3df(-maxBound * space - halfSpace, 0.0f, i * space));

				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative = rot270 * scaleMat;
				transform->Relative.setTranslation(core::vector3df(maxBound * space + halfSpace, 0.0f, i * space));
			}
		}
	}

	{
		CEntityPrefab* prefab = meshManager->loadModel("SampleBoids/Environment/1_Border_Corner.fbx", NULL, true);
		if (prefab)
		{
			CGameObject* corner = zone->createEmptyObject();
			CRenderMeshInstancing* meshInstancing = corner->addComponent<CRenderMeshInstancing>();
			meshInstancing->initFromPrefab(prefab);
			meshInstancing->initMaterial(envMaterials);

			CEntity* entity;
			CWorldTransformData* transform;

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative.setTranslation(core::vector3df(-maxBound * space - halfSpace, 0.0f, -maxBound * space - halfSpace));
			transform->Relative.setScale(scaleVector);

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative = rot90 * scaleMat;
			transform->Relative.setTranslation(core::vector3df(-maxBound * space - halfSpace, 0.0f, maxBound * space + halfSpace));

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative = rot180 * scaleMat;
			transform->Relative.setTranslation(core::vector3df(maxBound * space + halfSpace, 0.0f, maxBound * space + halfSpace));

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative = rot270 * scaleMat;
			transform->Relative.setTranslation(core::vector3df(maxBound * space + halfSpace, 0.0f, -maxBound * space - halfSpace));

			corner->addComponent<CIndirectLighting>();

			m_allObjects.push_back(corner);
		}
	}
}

void CViewInit::initCrowdByAnimTexture(CZone* zone, float envMin, float envMax, std::vector<CAnimationClip*>& clips, int fps)
{
	// bake clip animation	
	float maxDuration = 0.0f;
	for (CAnimationClip* clip : clips)
	{
		if (clip->Duration > maxDuration)
			maxDuration = clip->Duration;
	}

	std::vector<std::string> searchTextureFolders;

	CMaterialManager* materialManager = CMaterialManager::getInstance();
	CMeshManager* meshManager = CMeshManager::getInstance();

	CEntityPrefab* modelPrefab1 = meshManager->loadModel("SampleBoids/RifleMan/RifleMan.fbx", NULL, true);
	CEntityPrefab* modelPrefab2 = meshManager->loadModel("SampleBoids/RifleMan/RifleMan_ELITE.fbx", NULL, true);

	if (modelPrefab1 != NULL && modelPrefab2 != NULL)
	{
		CGameObject* character = zone->createEmptyObject();
		character->setName("Rifle");

		// render mesh & init material
		CRenderMesh* characterRenderer = character->addComponent<CRenderMesh>();
		characterRenderer->initFromPrefab(modelPrefab1);

		ArrayMaterial materials = materialManager->loadMaterial("SampleBoids/RifleMan/RifleMan.mat", true, searchTextureFolders);
		characterRenderer->initMaterial(materials);

		// init animation
		CAnimationController* animController = character->addComponent<CAnimationController>();
		CSkeleton* skeleton = animController->createSkeleton();

		// get bone map transform
		std::map<std::string, int> boneMap;
		skeleton->getBoneIdMap(boneMap);
		int numBones = (int)boneMap.size();

		int clipId = 0;
		int totalFrames = (int)(maxDuration * fps);
		int numClip = (int)clips.size();

		// build the matrix animations
		//
		//  [anim1]   frame0    frame1    frame2    ...
		//  bone1
		//  bone2
		//  bone3
		//  ...
		//  [anim2]   frame0    frame1    frame2    ...
		//  bone1
		//  bone2
		//  bone3
		//  ...
		core::matrix4* animationData = new core::matrix4[totalFrames * numBones * numClip];
		core::matrix4* transforms = new core::matrix4[numBones];

		for (CAnimationClip* clip : clips)
		{
			skeleton->setAnimation(clip, true);

			int clipFrames = (int)(clip->Duration * fps);
			int clipOffset = clipId * numBones * totalFrames;

			for (int i = 0; i < clipFrames; i++)
			{
				float t = i / (float)clipFrames;
				skeleton->simulateTransform(t * clip->Duration, core::IdentityMatrix, transforms, numBones);

				for (int j = 0; j < numBones; j++)
				{
					animationData[clipOffset + j * totalFrames + i] = transforms[j];
				}
			}

			clipId++;
		}

		character->remove();


		// create gpu anim character
		CGameObject* crowd1 = zone->createEmptyObject();
		crowd1->setName("Crowd1");

		CGameObject* crowd2 = zone->createEmptyObject();
		crowd2->setName("Crowd2");

		initSkinnedCrowd(crowd1, modelPrefab1, animationData, totalFrames, numBones * numClip, boneMap);
		initSkinnedCrowd(crowd2, modelPrefab2, animationData, totalFrames, numBones * numClip, boneMap);

		CRenderSkinnedInstancing* crowdSkinnedMesh1 = crowd1->getComponent<CRenderSkinnedInstancing>();
		CRenderSkinnedInstancing* crowdSkinnedMesh2 = crowd2->getComponent<CRenderSkinnedInstancing>();

		// spawn bold to test
		CEntity* entity;
		float space = 2.0f;
		float offset = space * 0.5f;
		float time = 0.0f;

		CBoidData* bold;
		CWorldTransformData* transform;
		core::vector3df position;

		float x, z;
		float dx = envMax - envMin;
		float dz = envMax - envMin;
		int count = 100;

		for (int i = 0; i < count / 2; i++)
		{
			// MODEL 1
			entity = crowdSkinnedMesh1->spawn();

			// random time
			time = os::Randomizer::frand();

			// set animation
			crowdSkinnedMesh1->setAnimation(entity, 0, clips[0], time * clips[0]->Duration, fps, 0);
			crowdSkinnedMesh1->setAnimation(entity, 1, clips[1], time * clips[1]->Duration, fps, 1);

			crowdSkinnedMesh1->setAnimationWeight(entity, 0, 1.0f);
			crowdSkinnedMesh1->setAnimationWeight(entity, 1, 0.0f);

			// set position
			x = envMin + os::Randomizer::frand() * dx;
			z = envMin + os::Randomizer::frand() * dz;

			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			position.set(x, 0.0f, z);

			transform->Relative.setTranslation(position);

			// add bold data
			bold = entity->addData<CBoidData>();
			bold->Location = position;


			// MODEL 2
			entity = crowdSkinnedMesh2->spawn();

			// random time
			time = os::Randomizer::frand();

			// set animation
			crowdSkinnedMesh2->setAnimation(entity, 0, clips[0], time * clips[0]->Duration, fps, 0);
			crowdSkinnedMesh2->setAnimation(entity, 1, clips[1], time * clips[1]->Duration, fps, 1);

			crowdSkinnedMesh2->setAnimationWeight(entity, 0, 1.0f);
			crowdSkinnedMesh2->setAnimationWeight(entity, 1, 0.0f);

			// set position
			x = envMin + os::Randomizer::frand() * dx;
			z = envMin + os::Randomizer::frand() * dz;

			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			position.set(x, 0.0f, z);

			transform->Relative.setTranslation(position);

			// add bold data
			bold = entity->addData<CBoidData>();
			bold->Location = position;
		}

		// free data
		delete[]animationData;
		delete[]transforms;
	}
}

void CViewInit::initSkinnedCrowd(CGameObject* crowd, CEntityPrefab* modelPrefab, core::matrix4* animationData, u32 w, u32 h, std::map<std::string, int>& bones)
{
	CRenderSkinnedInstancing* crowdSkinnedMesh;
	crowdSkinnedMesh = crowd->addComponent<CRenderSkinnedInstancing>();
	crowdSkinnedMesh->initFromPrefab(modelPrefab);
	crowdSkinnedMesh->initTextureTransform(animationData, w, h, bones);

	// It may be more optimal memory, but it hasn't been thoroughly tested in many cases
	crowdSkinnedMesh->applyShareTransformBuffer();
	crowdSkinnedMesh->applyShareMaterialBuffer();

	// body
	ArrayMaterial material = CMaterialManager::getInstance()->initDefaultMaterial(modelPrefab);
	material[0]->changeShader("BuiltIn/Shader/Toon/SkinToonInstancing2.xml");
	material[0]->setUniformTexture("uTexDiffuse", CTextureManager::getInstance()->getTexture("SampleBoids/Textures/CharacterAtlas.png"));
	material[0]->setUniformTexture("uTexRamp", CTextureManager::getInstance()->getTexture("BuiltIn/Textures/TCP2Ramp.png"));
	material[0]->autoDetectLoadTexture();

	crowdSkinnedMesh->initMaterial(material);

	crowd->addComponent<CIndirectLighting>();

	m_allObjects.push_back(crowd);
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
		listBundles.push_back("SampleBoids.zip");
		listBundles.push_back("SampleBoidsResource.zip");

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

			// hide all
			for (CGameObject* obj : m_allObjects)
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

			// hide all
			for (CGameObject* obj : m_allObjects)
				obj->setVisible(true);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
