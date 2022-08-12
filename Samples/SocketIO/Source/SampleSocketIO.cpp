#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleSocketIO.h"

#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSocketIO* app = new SampleSocketIO();
	getApplication()->registerAppEvent("SampleSocketIO", app);
}

SampleSocketIO::SampleSocketIO() :
	m_scene(NULL),
	m_forwardRP(NULL)
#if defined(USE_FREETYPE)	
	, m_largeFont(NULL)
#endif
{

}

SampleSocketIO::~SampleSocketIO()
{
	delete m_scene;
#if defined(USE_FREETYPE)	
	delete m_largeFont;
#endif
	delete m_forwardRP;

	delete m_io;
}

void SampleSocketIO::onInitApp()
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
	textLarge->setText("SampleSocketIO");
	textLarge->setTextAlign(CGUIElement::Left, CGUIElement::Top);
#endif

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);

	// see Readme.MD on Samples\SocketIOServer
	// that how to start the server
	m_io = new CSocketIO("localhost:8080");
	m_io->init();
}

void SampleSocketIO::onUpdate()
{
	// update application
	m_scene->update();

	m_io->update();

	if (m_io->isConnected())
	{
		static float testEmit = 0.0f;
		testEmit = testEmit - getTimeStep();
		if (testEmit < 0.0f)
		{
			testEmit = 10000.0f;

			// send event without param
			m_io->emit("hello", true, 1);

			// send event with 1 param
			m_io->emit("helloParam", "param", "\"testParam\"", true, 2);

			// send event with many params
			std::map<std::string, std::string> params;
			params["param0"] = "0";
			params["param1"] = "1";
			params["param2"] = "\"string\"";
			m_io->emit("helloParam", params, true, 3);
		}
	}
}

void SampleSocketIO::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleSocketIO::onPostRender()
{
	// post render application
}

bool SampleSocketIO::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleSocketIO::onResize(int w, int h)
{
	// on window size changed
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleSocketIO::onResume()
{
	// resume application
}

void SampleSocketIO::onPause()
{
	// pause application
}

void SampleSocketIO::onQuitApp()
{
	// end application
	delete this;
}