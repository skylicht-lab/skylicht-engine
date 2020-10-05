#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleDrawPrimitives.h"

#include "CCubeComponent.h"
#include "CCubeRotate.h"

#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleDrawPrimitives *app = new SampleDrawPrimitives();
	getApplication()->registerAppEvent("SampleDrawPrimitives", app);
}

SampleDrawPrimitives::SampleDrawPrimitives() :
	m_scene(NULL)
{

}

SampleDrawPrimitives::~SampleDrawPrimitives()
{
	delete m_scene;
	delete m_forwardRP;
}

void SampleDrawPrimitives::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

	// Load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// Create a Scene
	m_scene = new CScene();

	// Create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// Create 2D camera
	CGameObject *guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// Create 3d camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3D grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// Create cube 1
	CGameObject *cubeObj1 = zone->createEmptyObject();
	cubeObj1->addComponent<CCubeComponent>();
	cubeObj1->addComponent<CCubeRotate>()->setRotate(0.1f, 0.1f, 0.1f);
	cubeObj1->getTransformEuler()->setPosition(core::vector3df(-1.0f, 0.0f, 0.0f));

	// Create cube 2
	CGameObject *cubeObj2 = zone->createEmptyObject();
	cubeObj2->addComponent<CCubeComponent>();
	cubeObj2->addComponent<CCubeRotate>()->setRotate(0.0f, 0.1f, 0.0f);
	cubeObj2->getTransformEuler()->setPosition(core::vector3df(1.0f, 0.0f, 0.0f));

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleDrawPrimitives::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleDrawPrimitives::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render 2d gui
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleDrawPrimitives::onPostRender()
{
	// post render application
}

bool SampleDrawPrimitives::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleDrawPrimitives::onResume()
{
	// resume application
}

void SampleDrawPrimitives::onPause()
{
	// pause application
}

void SampleDrawPrimitives::onQuitApp()
{
	// end application
	delete this;
}