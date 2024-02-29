#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

#include "AnimationDefine.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true),
	m_character(NULL)
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

	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/ReflectionProbe.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkin.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkinAlpha.xml");

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

	// crate gui camera
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
	camera->setPosition(core::vector3df(1.8f, 1.8f, 2.0f));
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
	CGameObject* grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// load dae animation
	CAnimationManager* animManager = CAnimationManager::getInstance();

	// skinned mesh
	CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel("SampleAnimations/HeroArtwork/Hero.dae", "SampleAnimations/HeroArtwork/Textures");
	if (prefab != NULL)
	{
		std::vector<std::string> textureFolders;
		ArrayMaterial material = CMaterialManager::getInstance()->loadMaterial("SampleAnimations/HeroArtwork/Hero.mat", true, textureFolders);

		// create character object
		m_character = zone->createEmptyObject();
		m_character->setName(L"Character");

		// load skinned mesh character
		CRenderMesh* renderMesh1 = m_character->addComponent<CRenderMesh>();
		renderMesh1->initFromPrefab(prefab);
		renderMesh1->initMaterial(material);

		// apply animation to character
		CAnimationController* animController = m_character->addComponent<CAnimationController>();

		// create multi skeleton
		for (int i = 0; i <= (int)EAnimationId::Result; i++)
		{
			CSkeleton* skeleton = animController->createSkeleton();
			skeleton->getTimeline().Weight = 0.0f;
		}

		std::string animationName[(int)EAnimationId::Result];

		animationName[(int)EAnimationId::Idle] = "Hero@Idle.dae";
		animationName[(int)EAnimationId::WalkBack] = "Hero@WalkBackward.dae";
		animationName[(int)EAnimationId::WalkForward] = "Hero@WalkForward.dae";
		animationName[(int)EAnimationId::WalkLeft] = "Hero@WalkStrafeLeft.dae";
		animationName[(int)EAnimationId::WalkRight] = "Hero@WalkStrafeRight.dae";
		animationName[(int)EAnimationId::RunBack] = "Hero@RunBackward.dae";
		animationName[(int)EAnimationId::RunForward] = "Hero@RunForward.dae";
		animationName[(int)EAnimationId::RunLeft] = "Hero@RunStrafeLeft.dae";
		animationName[(int)EAnimationId::RunRight] = "Hero@RunStrafeRight.dae";
		animationName[(int)EAnimationId::Aim] = "Hero@AimStraight.dae";

#define GET_SKELETON(name) (animController->getSkeleton((int)(name)))

		for (int i = 0; i < (int)EAnimationId::Result; i++)
		{
			if (animationName->empty())
				continue;

			std::string anim = "SampleAnimations/HeroArtwork/";
			anim += animationName[i];

			// set loop animation for skeleton
			CAnimationClip* clip = animManager->loadAnimation(anim.c_str());
			if (clip != NULL)
				animController->getSkeleton(i)->setAnimation(clip, true);
		}

		CSkeleton* movement = GET_SKELETON(EAnimationId::Movement);
		CSkeleton* idle = GET_SKELETON(EAnimationId::Idle);
		CSkeleton* result = GET_SKELETON(EAnimationId::Result);

		// idle
		idle->setTarget(movement);
		idle->getTimeline().Weight = 1.0f;

		// walk
		for (int i = (int)EAnimationId::WalkBack; i <= (int)EAnimationId::WalkRight; i++)
			animController->getSkeleton(i)->setTarget(movement);

		// run
		for (int i = (int)EAnimationId::RunBack; i <= (int)EAnimationId::RunRight; i++)
			animController->getSkeleton(i)->setTarget(movement);

		// movement
		movement->setTarget(result);
		movement->setAnimationType(CSkeleton::Blending);
		movement->getTimeline().Weight = 1.0f;
		// movement->setJointWeights("Spine3", 0.0f, true);

		// aim
		/*
		CSkeleton* aim = GET_SKELETON(EAnimationId::Aim)
		aim->setTarget(result);
		aim->setJointWeights(0.0f); // clear all weight
		aim->setJointWeights("Spine3", 1.0f, true); // aim from "Spine3"
		*/

		// result
		result->setAnimationType(CSkeleton::Blending);

		// final output
		animController->setOutput(result);

		// set position for character
		m_character->getTransformEuler()->setPosition(core::vector3df(0.0f, 0.0f, 0.0f));

		// set sh lighting
		CIndirectLighting* indirectLighting = m_character->addComponent<CIndirectLighting>();
		indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);
	}

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();
	CForwardRP* fwrp = new CForwardRP();
	fwrp->initRender(w, h);

	context->setCustomRP(fwrp);
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
		listBundles.push_back("SampleAnimations.zip");
		listBundles.push_back("SampleAnimationsResource.zip");
		listBundles.push_back(getApplication()->getTexturePackageName("SampleAnimations").c_str());

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

			m_character->setVisible(false);

			CGameObject* bakeCameraObj = scene->getZone(0)->createEmptyObject();
			CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
			scene->updateAddRemoveObject();

			core::vector3df pos(0.0f, 0.0f, 0.0f);

			core::vector3df normal = CTransform::s_oy;
			core::vector3df tangent = CTransform::s_ox;
			core::vector3df binormal = normal.crossProduct(tangent);
			binormal.normalize();

			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);
			Lightmapper::CSH9 sh = lm->bakeAtPosition(
				bakeCamera,
				rp,
				scene->getEntityManager(),
				pos,
				normal, tangent, binormal);

			// apply indirect lighting
			m_character->getComponent<CIndirectLighting>()->setSH(sh.getValue());
			m_character->setVisible(true);
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
