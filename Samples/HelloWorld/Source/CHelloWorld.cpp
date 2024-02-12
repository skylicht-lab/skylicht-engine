#include "pch.h"
#include "SkylichtEngine.h"
#include "CHelloWorld.h"

void installApplication(const std::vector<std::string>& argv)
{
	CHelloWorld *demo = new CHelloWorld();
	getApplication()->registerAppEvent("CHelloWorld", demo);
}

CHelloWorld::CHelloWorld() :
	m_scene(NULL),
	m_largeFont(NULL)
{

}

CHelloWorld::~CHelloWorld()
{
	delete m_scene;
	delete m_largeFont;
}

void CHelloWorld::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();
	
	// Show console
	app->showDebugConsole();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

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

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 50);

	// Create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// Create UI Text in Canvas
	CGUIText *textLarge = canvas->createText(m_largeFont);
    textLarge->setDock(EGUIDock::DockFill);
	textLarge->setText("Hello, World");
	textLarge->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
}

void CHelloWorld::onUpdate()
{
	// update application
	m_scene->update();
}

void CHelloWorld::onRender()
{
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void CHelloWorld::onPostRender()
{
	// post render application
}

bool CHelloWorld::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void CHelloWorld::onResize(int w, int h)
{

}

void CHelloWorld::onResume()
{
	// resume application
}

void CHelloWorld::onPause()
{
	// pause application
}

void CHelloWorld::onQuitApp()
{
	// end application
	delete this;
}
