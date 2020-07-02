#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleMaterials.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleMaterials *app = new SampleMaterials();
	getApplication()->registerAppEvent("SampleMaterials", app);
}

SampleMaterials::SampleMaterials() :
	m_scene(NULL)
{

}

SampleMaterials::~SampleMaterials()
{
	delete m_scene;
}

void SampleMaterials::onInitApp()
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
}

void SampleMaterials::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleMaterials::onRender()
{
	// Render hello,world text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleMaterials::onPostRender()
{
	// post render application
}

bool SampleMaterials::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleMaterials::onResume()
{
	// resume application
}

void SampleMaterials::onPause()
{
	// pause application
}

void SampleMaterials::onQuitApp()
{
	// end application
	delete this;
}