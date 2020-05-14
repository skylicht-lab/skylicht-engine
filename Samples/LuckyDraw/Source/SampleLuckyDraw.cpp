#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleLuckyDraw.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLuckyDraw *app = new SampleLuckyDraw();
	getApplication()->registerAppEvent("SampleLuckyDraw", app);
}

SampleLuckyDraw::SampleLuckyDraw() :
	m_scene(NULL),
	m_largeFont(NULL)
{

}

SampleLuckyDraw::~SampleLuckyDraw()
{
	delete m_scene;
	delete m_largeFont;
}

void SampleLuckyDraw::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("LuckyDraw.zip"), false, false);

	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");
	freetypeFont->initFont("LasVegas", "LuckyDraw/LasVegasJackpotRegular.otf");

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

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("LasVegas", 50);

	// Create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// Create UI Text in Canvas
	CGUIText *textLarge = canvas->createText(m_largeFont);
	textLarge->setText("SampleLuckyDraw");
	textLarge->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
}

void SampleLuckyDraw::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleLuckyDraw::onRender()
{
	// Render hello,world text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleLuckyDraw::onPostRender()
{
	// post render application
}

bool SampleLuckyDraw::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleLuckyDraw::onResume()
{
	// resume application
}

void SampleLuckyDraw::onPause()
{
	// pause application
}

void SampleLuckyDraw::onQuitApp()
{
	// end application
	delete this;
}