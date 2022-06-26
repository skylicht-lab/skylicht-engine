#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleInstancing.h"

#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleInstancing* app = new SampleInstancing();
	getApplication()->registerAppEvent("SampleInstancing", app);
}

SampleInstancing::SampleInstancing() :
	m_scene(NULL),
	m_forwardRP(NULL)
{

}

SampleInstancing::~SampleInstancing()
{
	delete m_scene;
	delete m_forwardRP;
}

void SampleInstancing::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

	// load basic shader
	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// create a Scene
	m_scene = new CScene();

	// create a Zone in Scene
	CZone* zone = m_scene->createZone();

	// create 2D camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// create 3D camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	CGameObject* grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleInstancing::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleInstancing::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleInstancing::onPostRender()
{
	// post render application
}

bool SampleInstancing::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleInstancing::onResize(int w, int h)
{
	// on window size changed
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleInstancing::onResume()
{
	// resume application
}

void SampleInstancing::onPause()
{
	// pause application
}

void SampleInstancing::onQuitApp()
{
	// end application
	delete this;
}