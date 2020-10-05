#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleSkinnedMesh.h"

#include "SkyDome/CSkyDome.h"
#include "GridPlane/CGridPlane.h"

#include "Lightmapper/CLightmapper.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSkinnedMesh *app = new SampleSkinnedMesh();
	getApplication()->registerAppEvent("SampleSkinnedMesh", app);
}

SampleSkinnedMesh::SampleSkinnedMesh() :
	m_scene(NULL),
	m_camera(NULL),
	m_guiCamera(NULL),
	m_forwardRP(NULL),
	m_bakeSHLighting(true),
	m_character01(NULL),
	m_character02(NULL)
{
	Lightmapper::CLightmapper::createGetInstance();
}

SampleSkinnedMesh::~SampleSkinnedMesh()
{
	delete m_scene;
	delete m_forwardRP;

	Lightmapper::CLightmapper::releaseInstance();
}

void SampleSkinnedMesh::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	io::IFileSystem* fs = app->getFileSystem();
	fs->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("Common.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("SampleModels.zip"), false, false);

	// Load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/ReflectionProbe.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkin.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkinAlpha.xml");

	// Create a Scene
	m_scene = new CScene();

	// Create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	m_camera->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// gui camera
	CGameObject *guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// reflection probe
	CGameObject *reflectionProbeObj = zone->createEmptyObject();
	CReflectionProbe *reflection = reflectionProbeObj->addComponent<CReflectionProbe>();
	reflection->loadStaticTexture("Common/Textures/Sky/PaperMill");

	// 3D grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	lightObj->addComponent<CDirectionalLight>();

	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// load dae animation
	CAnimationManager *animManager = CAnimationManager::getInstance();
	CAnimationClip *clip1 = animManager->loadAnimation("SampleModels/MixamoCharacter/Hip_Hop_Dancing.dae");
	CAnimationClip *clip2 = animManager->loadAnimation("SampleModels/MixamoCharacter/Samba_Dancing.dae");

	// skinned mesh
	CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel("SampleModels/MixamoCharacter/Ch17_nonPBR.dae", "SampleModels/MixamoCharacter/textures");
	if (prefab != NULL)
	{
		// test exporter
		/*
		CMeshManager::getInstance()->exportModel(
			prefab->getEntities(),
			prefab->getNumEntities(),
			"../Assets/SkinnedMesh/Ch17_nonPBR.smesh");
		*/

		ArrayMaterial material = CMaterialManager::getInstance()->initDefaultMaterial(prefab);
		if (material.size() == 2)
		{
			// body
			material[1]->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkin.xml");
			material[1]->autoDetectLoadTexture();

			// hair
			material[0]->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkinAlpha.xml");
			material[0]->autoDetectLoadTexture();
		}

		// CHARACTER 01
		// ------------------------------------------

		// create character 01 object
		m_character01 = zone->createEmptyObject();

		// load skinned mesh character 01
		CRenderMesh *renderMesh1 = m_character01->addComponent<CRenderMesh>();
		renderMesh1->initFromPrefab(prefab);
		renderMesh1->initMaterial(material);

		// apply animation to character 01
		CAnimationController *animController1 = m_character01->addComponent<CAnimationController>();
		CSkeleton *skeleton1 = animController1->createSkeleton();
		skeleton1->setAnimation(clip1, true);

		// set position for character 01
		m_character01->getTransformEuler()->setPosition(core::vector3df(-1.0f, 0.0f, 0.0f));

		// set sh lighting
		CIndirectLighting *indirectLighting = m_character01->addComponent<CIndirectLighting>();
		indirectLighting->setIndirectLightingType(CIndirectLighting::SH4);


		// CHARACTER 02
		// ------------------------------------------

		// create character 02 object
		m_character02 = zone->createEmptyObject();

		// load skinned mesh character 02
		CRenderMesh *renderMesh2 = m_character02->addComponent<CRenderMesh>();
		renderMesh2->initFromPrefab(prefab);
		renderMesh2->initMaterial(material);

		// apply animation to character 02
		CAnimationController *animController2 = m_character02->addComponent<CAnimationController>();
		CSkeleton *skeleton2 = animController2->createSkeleton();
		skeleton2->setAnimation(clip2, true);

		// set position for character 02
		m_character02->getTransformEuler()->setPosition(core::vector3df(1.0f, 0.0f, 0.0f));

		// set sh lighting
		indirectLighting = m_character02->addComponent<CIndirectLighting>();
		indirectLighting->setIndirectLightingType(CIndirectLighting::SH4);
	}

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleSkinnedMesh::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleSkinnedMesh::onRender()
{
	if (m_bakeSHLighting == true)
	{
		m_bakeSHLighting = false;

		m_character01->setVisible(false);
		m_character02->setVisible(false);

		CGameObject *bakeCameraObj = m_scene->getZone(0)->createEmptyObject();
		CCamera *bakeCamera = bakeCameraObj->addComponent<CCamera>();
		m_scene->updateAddRemoveObject();

		core::vector3df pos(0.0f, 0.0f, 0.0f);

		core::vector3df normal = CTransform::s_oy;
		core::vector3df tangent = CTransform::s_ox;
		core::vector3df binormal = normal.crossProduct(tangent);
		binormal.normalize();

		Lightmapper::CLightmapper *lm = Lightmapper::CLightmapper::getInstance();
		lm->initBaker(64);
		Lightmapper::CSH9 sh = lm->bakeAtPosition(
			bakeCamera,
			m_forwardRP,
			m_scene->getEntityManager(),
			pos,
			normal, tangent, binormal);

		// apply indirect lighting
		m_character01->getComponent<CIndirectLighting>()->setSH(sh.getValue());
		m_character02->getComponent<CIndirectLighting>()->setSH(sh.getValue());

		m_character01->setVisible(true);
		m_character02->setVisible(true);
	}

	m_forwardRP->render(
		NULL,
		m_camera,
		m_scene->getZone(0)->getEntityManager(),
		core::recti()
	);

	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleSkinnedMesh::onPostRender()
{
	// post render application
}

bool SampleSkinnedMesh::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleSkinnedMesh::onResume()
{
	// resume application
}

void SampleSkinnedMesh::onPause()
{
	// pause application
}

void SampleSkinnedMesh::onQuitApp()
{
	// end application
	delete this;
}