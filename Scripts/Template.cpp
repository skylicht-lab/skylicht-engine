#include "pch.h"
#include "@project_name@.h"

#include "Material/Shader/CShaderManager.h"

// font - freetype
#if defined(USE_FREETYPE)
#include "Graphics2D/Glyph/CGlyphFreetype.h"
#include "Graphics2D/SpriteFrame/CGlyphFont.h"
#endif

// canvas 2D
#include "Graphics2D/CCanvas.h"
#include "Graphics2D/CGraphics2D.h"

void installApplication(const std::vector<std::string>& argv)
{
	@project_name@ *app = new @project_name@();
	getApplication()->registerAppEvent("@project_name@", app);
}

@project_name@::@project_name@() :
	m_scene(NULL),
	m_largeFont(NULL)
{

}

@project_name@::~@project_name@()
{
	delete m_scene;
	delete m_largeFont;
}

void @project_name@::onInitApp()
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
	textLarge->setText("@project_name@");
	textLarge->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
#endif
}

void @project_name@::onUpdate()
{
	// update application
	m_scene->update();
}

void @project_name@::onRender()
{
	// Render hello,world text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void @project_name@::onPostRender()
{
	// post render application
}

bool @project_name@::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void @project_name@::onResume()
{
	// resume application
}

void @project_name@::onPause()
{
	// pause application
}

void @project_name@::onQuitApp()
{
	// end application
	delete this;
}