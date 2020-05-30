#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleLightmapUV.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmapUV *app = new SampleLightmapUV();
	getApplication()->registerAppEvent("SampleLightmapUV", app);
}

SampleLightmapUV::SampleLightmapUV() :
	m_scene(NULL)
{

}

SampleLightmapUV::~SampleLightmapUV()
{
	delete m_scene;
}

void SampleLightmapUV::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("LightmapUV.zip"), false, false);

	// Load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// Create a Scene
	m_scene = new CScene();

	// Create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// Create 2D camera
	CGameObject *guiCameraObject = (CGameObject*)zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);
}

void SampleLightmapUV::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleLightmapUV::onRender()
{
	// Render hello,world text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleLightmapUV::onPostRender()
{
	// post render application
}

bool SampleLightmapUV::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleLightmapUV::onResume()
{
	// resume application
}

void SampleLightmapUV::onPause()
{
	// pause application
}

void SampleLightmapUV::onQuitApp()
{
	// end application
	delete this;
}