#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "Primitive/CPlane.h"
#include "SkyDome/CSkyDome.h"

#include "Bolds/CBoldSystem.h"
#include "Bolds/CBoldAnimationSystem.h"

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

	// camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	CCamera* camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
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

	// 3D grid
	m_plane = zone->createEmptyObject();
	CPlane* plane = m_plane->addComponent<CPlane>();
	plane->getMaterial()->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetersGrid.xml");
	m_plane->getTransformEuler()->setScale(core::vector3df(50.0f, 1.0f, 50.0f));

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
	CAnimationManager* animManager = CAnimationManager::getInstance();

	CAnimationClip* animIdle = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Idle.fbx");
	CAnimationClip* animWalk = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Walk.fbx");
	CAnimationClip* animRun = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Running.fbx");

	std::vector<CAnimationClip*> clips;
	clips.push_back(animIdle);
	clips.push_back(animWalk);
	clips.push_back(animRun);

	float maxDuration = 0.0f;
	for (CAnimationClip* clip : clips)
	{
		if (clip->Duration > maxDuration)
			maxDuration = clip->Duration;
	}

	CEntityPrefab* modelPrefab1 = meshManager->loadModel("SampleBoids/RifleMan/RifleMan.fbx", NULL, true);
	CEntityPrefab* modelPrefab2 = meshManager->loadModel("SampleBoids/RifleMan/RifleMan_ELITE.fbx", NULL, true);

	if (modelPrefab1 != NULL && modelPrefab2 != NULL)
	{
		// create cat
		CGameObject* rifle = zone->createEmptyObject();
		rifle->setName("Rifle");

		// render mesh & init material
		CRenderMesh* rifleRenderer = rifle->addComponent<CRenderMesh>();
		rifleRenderer->initFromPrefab(modelPrefab1);

		std::vector<std::string> folders;
		ArrayMaterial materials = CMaterialManager::getInstance()->loadMaterial("SampleBoids/RifleMan/RifleMan.mat", true, folders);
		rifleRenderer->initMaterial(materials);

		// init animation
		CAnimationController* animController = rifle->addComponent<CAnimationController>();
		CSkeleton* skeleton = animController->createSkeleton();

		// get bone map transform
		std::map<std::string, int> boneMap;
		skeleton->getBoneIdMap(boneMap);
		int numBones = (int)boneMap.size();

		int fps = 60;
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

		rifle->remove();

		// create gpu anim character
		m_crowd1 = zone->createEmptyObject();
		m_crowd2 = zone->createEmptyObject();

		initCrowd(m_crowd1, modelPrefab1, animationData, totalFrames, numBones * numClip, boneMap);
		initCrowd(m_crowd2, modelPrefab2, animationData, totalFrames, numBones * numClip, boneMap);

		CRenderSkinnedInstancing* crowdSkinnedMesh1 = m_crowd1->getComponent<CRenderSkinnedInstancing>();
		CRenderSkinnedInstancing* crowdSkinnedMesh2 = m_crowd2->getComponent<CRenderSkinnedInstancing>();

		// spawn bold to test
		CEntity* entity;
		float space = 2.0f;
		float offset = space * 0.5f;
		float time = 0.0f;

		CBoldData* bold;
		CWorldTransformData* transform;
		core::vector3df position;

		for (int i = -6; i < 6; i++)
		{
			for (int j = -6; j < 6; j++)
			{
				// MODEL 1
				entity = crowdSkinnedMesh1->spawn();

				// random time
				time = os::Randomizer::frand();

				// set animation
				crowdSkinnedMesh1->setAnimation(entity, 0, animIdle, time * animIdle->Duration, fps, 0);
				crowdSkinnedMesh1->setAnimation(entity, 1, animWalk, time * animWalk->Duration, fps, 1);

				crowdSkinnedMesh1->setAnimationWeight(entity, 0, 1.0f);
				crowdSkinnedMesh1->setAnimationWeight(entity, 1, 0.0f);

				// set position
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				position.set(i * space, 0.0f, j * space);

				transform->Relative.setTranslation(position);

				// add bold data
				bold = entity->addData<CBoldData>();
				bold->Location = position;


				// MODEL 2
				entity = crowdSkinnedMesh2->spawn();

				// random time
				time = os::Randomizer::frand();

				// set animation
				crowdSkinnedMesh2->setAnimation(entity, 0, animIdle, time * animIdle->Duration, fps, 0);
				crowdSkinnedMesh2->setAnimation(entity, 1, animWalk, time * animWalk->Duration, fps, 1);

				crowdSkinnedMesh2->setAnimationWeight(entity, 0, 1.0f);
				crowdSkinnedMesh2->setAnimationWeight(entity, 1, 0.0f);

				// set position
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				position.set(i * space + offset, 0.0f, j * space + offset);

				transform->Relative.setTranslation(position);

				// add bold data
				bold = entity->addData<CBoldData>();
				bold->Location = position;
			}
		}

		// free data
		delete[]animationData;
		delete[]transforms;

		// add bold system (that will update moving for CBoldData)
		scene->getEntityManager()->addSystem<CBoldSystem>();

		// add bold system (that will update animation clip)
		CBoldAnimationSystem* animSystem = scene->getEntityManager()->addSystem<CBoldAnimationSystem>();
		animSystem->addClip(animIdle, 0, fps, 0.0f);
		animSystem->addClip(animWalk, 1, fps, 0.01f);
		animSystem->addClip(animRun, 2, fps, 0.03f);
	}

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initRenderPipeline(w, h);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);

	// Test use 2 cascade shadow
	// context->getShadowMapRenderPipeline()->setShadowCascade(2);

	// Test no shadow cascade (30m far shadow)
	context->getShadowMapRenderPipeline()->setNoShadowCascade(2048, 30.0f);
}

void CViewInit::initCrowd(CGameObject* crowd, CEntityPrefab* modelPrefab, core::matrix4* animationData, u32 w, u32 h, std::map<std::string, int>& bones)
{
	CRenderSkinnedInstancing* crowdSkinnedMesh;
	crowdSkinnedMesh = crowd->addComponent<CRenderSkinnedInstancing>();
	crowdSkinnedMesh->initFromPrefab(modelPrefab);
	crowdSkinnedMesh->initTextureTransform(animationData, w, h, bones);

	// applyShareInstancingBuffer: It may be more optimal memory, but it hasn't been thoroughly tested in many cases
	crowdSkinnedMesh->applyShareInstancingBuffer();

	// body
	ArrayMaterial material = CMaterialManager::getInstance()->initDefaultMaterial(modelPrefab);
	material[0]->changeShader("BuiltIn/Shader/Toon/SkinToonInstancing2.xml");
	material[0]->setUniformTexture("uTexDiffuse", CTextureManager::getInstance()->getTexture("SampleBoids/Texture/CharacterAtlas.png"));
	material[0]->setUniformTexture("uTexRamp", CTextureManager::getInstance()->getTexture("BuiltIn/Textures/TCP2Ramp.png"));
	material[0]->autoDetectLoadTexture();

	crowdSkinnedMesh->initMaterial(material);
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
#if defined(WINDOWS_STORE)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#elif defined(MACOS)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#else
			fileSystem->addFileArchive(r, false, false);
#endif
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
			scene->updateAddRemoveObject();

			// hide objects
			// that fix the probes ambient is brigther because plane color affect on it
			m_crowd1->setVisible(false);
			m_crowd2->setVisible(false);
			m_plane->setVisible(false);

			// bake light probe
			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);

			std::vector<CLightProbe*> probes;
			probes.push_back(lightProbe);

			lm->bakeProbes(probes, bakeCamera, rp, scene->getEntityManager());

			// show objects again
			m_crowd1->setVisible(true);
			m_crowd2->setVisible(true);
			m_plane->setVisible(true);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
