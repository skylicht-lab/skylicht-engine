#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleSkinnedMesh.h"

#include "SkyDome/CSkyDome.h"
#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSkinnedMesh *app = new SampleSkinnedMesh();
	getApplication()->registerAppEvent("SampleSkinnedMesh", app);
}

SampleSkinnedMesh::SampleSkinnedMesh() :
	m_scene(NULL),
	m_camera(NULL),
	m_guiCamera(NULL),
	m_forwardRP(NULL)
{

}

SampleSkinnedMesh::~SampleSkinnedMesh()
{
	delete m_scene;
	delete m_forwardRP;
}

void SampleSkinnedMesh::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	io::IFileSystem* fs = app->getFileSystem();
	fs->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("Common.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("SkinnedMesh.zip"), false, false);

	// Load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

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
	CGameObject *guiCameraObject = (CGameObject*)zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// 3D grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	lightObj->addComponent<CDirectionalLight>();

	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -7.0f, -1.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// load dae animation
	CAnimationManager *animManager = CAnimationManager::getInstance();
	CAnimationClip *clip1 = animManager->loadAnimation("SkinnedMesh/Hip_Hop_Dancing.dae");
	CAnimationClip *clip2 = animManager->loadAnimation("SkinnedMesh/Samba_Dancing.dae");

	// skinned mesh
	CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel("SkinnedMesh/Ch17_nonPBR.dae", "SkinnedMesh/textures");
	if (prefab != NULL)
	{
		// create character 01 object
		CGameObject *characterObj1 = zone->createEmptyObject();

		// load skinned mesh character 01
		CRenderMesh *renderMesh1 = characterObj1->addComponent<CRenderMesh>();
		renderMesh1->initFromPrefab(prefab);

		// apply animation to character 01
		CAnimationController *animController1 = characterObj1->addComponent<CAnimationController>();
		CSkeleton *skeleton1 = animController1->createSkeleton();
		skeleton1->setAnimation(clip1, true);

		// set position for character 01
		characterObj1->getTransformEuler()->setPosition(core::vector3df(-1.0f, 0.0f, 0.0f));



		// create character 02 object
		CGameObject *characterObj2 = zone->createEmptyObject();

		// load skinned mesh character 02
		CRenderMesh *renderMesh2 = characterObj2->addComponent<CRenderMesh>();
		renderMesh2->initFromPrefab(prefab);

		// apply animation to character 02
		CAnimationController *animController2 = characterObj2->addComponent<CAnimationController>();
		CSkeleton *skeleton2 = animController2->createSkeleton();
		skeleton2->setAnimation(clip2, true);

		// set position for character 02
		characterObj2->getTransformEuler()->setPosition(core::vector3df(1.0f, 0.0f, 0.0f));
	}


	// render pipeline
	m_forwardRP = new CForwardRP();
}

void SampleSkinnedMesh::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleSkinnedMesh::onRender()
{
	m_forwardRP->render(
		NULL,
		m_camera,
		m_scene->getZone(0)->getEntityManager(),
		core::recti()
	);

	// Render hello,world text in gui camera
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