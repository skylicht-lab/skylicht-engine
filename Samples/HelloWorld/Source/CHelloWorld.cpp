#include "pch.h"
#include "SkylichtEngine.h"
#include "CHelloWorld.h"

void installApplication(const std::vector<std::string>& argv)
{
	CHelloWorld *demo = new CHelloWorld();
	getApplication()->registerAppEvent("CHelloWorld", demo);
}

CHelloWorld::CHelloWorld() :
	m_scene(NULL)
#if defined(USE_FREETYPE)
	,m_largeFont(NULL)
#endif
{

}

CHelloWorld::~CHelloWorld()
{
	delete m_scene;
#if defined(USE_FREETYPE)
	delete m_largeFont;
#endif
}

void CHelloWorld::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

#if defined(USE_FREETYPE)
	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");
#endif

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

#if defined(USE_FREETYPE)
	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 50);

	// Create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// Create UI Text in Canvas
	CGUIText *textLarge = canvas->createText(m_largeFont);
	textLarge->setText("Hello, World");
	textLarge->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
#endif
}

void CHelloWorld::onUpdate()
{
	// update application
	m_scene->update();
}

void CHelloWorld::onRender()
{
	// Render hello,world text in gui camera
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
