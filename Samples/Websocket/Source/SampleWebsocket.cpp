#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleWebsocket.h"

#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleWebsocket* app = new SampleWebsocket();
	getApplication()->registerAppEvent("SampleWebsocket", app);
}

SampleWebsocket::SampleWebsocket() :
	m_scene(NULL),
	m_forwardRP(NULL)
#if defined(USE_FREETYPE)	
	, m_largeFont(NULL)
#endif
{

}

SampleWebsocket::~SampleWebsocket()
{
	delete m_scene;
#if defined(USE_FREETYPE)	
	delete m_largeFont;
#endif
	delete m_forwardRP;

	delete m_websocket;
}

void SampleWebsocket::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	app->showDebugConsole();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

#if defined(USE_FREETYPE)
	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");
#endif

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

#if defined(USE_FREETYPE)
	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 50);

	// create 2D Canvas
	CGameObject* canvasObject = zone->createEmptyObject();
	CCanvas* canvas = canvasObject->addComponent<CCanvas>();

	// create UI Text in Canvas
	CGUIText* textLarge = canvas->createText(m_largeFont);
	textLarge->setText("SampleWebsocket");
	textLarge->setTextAlign(CGUIElement::Left, CGUIElement::Top);
#endif

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);

	m_websocket = new CWebsocket("localhost:8080", this);
	m_websocket->init();
}

void SampleWebsocket::onWebSocketMessage(const std::string& data)
{
	os::Printer::log(data.c_str());
}

void SampleWebsocket::onUpdate()
{
	// update application
	m_scene->update();

	m_websocket->update();
}

void SampleWebsocket::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleWebsocket::onPostRender()
{
	// post render application
}

bool SampleWebsocket::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleWebsocket::onResize(int w, int h)
{
	// on window size changed
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleWebsocket::onResume()
{
	// resume application
}

void SampleWebsocket::onPause()
{
	// pause application
}

void SampleWebsocket::onQuitApp()
{
	// end application
	delete this;
}