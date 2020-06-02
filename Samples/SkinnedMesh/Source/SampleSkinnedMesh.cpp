#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleSkinnedMesh.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSkinnedMesh *app = new SampleSkinnedMesh();
	getApplication()->registerAppEvent("SampleSkinnedMesh", app);
}

SampleSkinnedMesh::SampleSkinnedMesh() :
	m_scene(NULL)
#if defined(USE_FREETYPE)	
	,m_largeFont(NULL)
#endif
{

}

SampleSkinnedMesh::~SampleSkinnedMesh()
{
	delete m_scene;
#if defined(USE_FREETYPE)	
	delete m_largeFont;
#endif
}

void SampleSkinnedMesh::onInitApp()
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
	textLarge->setText("SampleSkinnedMesh");
	textLarge->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
#endif
}

void SampleSkinnedMesh::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleSkinnedMesh::onRender()
{
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