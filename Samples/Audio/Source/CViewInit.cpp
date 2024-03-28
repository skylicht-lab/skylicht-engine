#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_largeFont(NULL),
	m_canvasObject(NULL),
	m_guiText(NULL),
	m_blinkTime(0.0f)
{

}

CViewInit::~CViewInit()
{
	m_canvasObject->remove();
	delete m_largeFont;

	// handle click event
	CEventManager::getInstance()->unRegisterEvent(this);
}

io::path CViewInit::getBuiltInPath(const char *name)
{
	return getApplication()->getBuiltInPath(name);
}

void CViewInit::onInit()
{
	CBaseApp* app = getApplication();
	app->showDebugConsole();
	app->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// handle click event
	CEventManager::getInstance()->registerEvent("ViewInit", this);
}

void CViewInit::initScene()
{
	CBaseApp* app = getApplication();

	// create a scene
	CScene *scene = CContext::getInstance()->initScene();
	CZone *zone = scene->createZone();

	// create 2D camera
	CGameObject *guiCameraObject = zone->createEmptyObject();
	CCamera *guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// create 3D camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	CCamera *camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, Transform::Oy);

	// 2d gui
	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 30);

	// create 2D Canvas
	m_canvasObject = zone->createEmptyObject();
	CCanvas *canvas = m_canvasObject->addComponent<CCanvas>();

	// create UI Text in Canvas
	m_guiText = canvas->createText(m_largeFont);
	m_guiText->setPosition(core::vector3df(0.0f, -40.0f, 0.0f));
	m_guiText->setText("Click to continue...");
	m_guiText->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Bottom);

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext *context = CContext::getInstance();
	CForwardRP  *fwrp = new CForwardRP();
	fwrp->initRender(w, h);

	context->setCustomRP(fwrp);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);
}

void CViewInit::onDestroy()
{

}

void CViewInit::onUpdate()
{
	CContext *context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();

		std::vector<std::string> listBundles;
		listBundles.push_back("SampleAudio.zip");

#ifdef __EMSCRIPTEN__
		const char *filename = listBundles[m_downloaded].c_str();

		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL(filename, filename);
			m_getFile->download(CGetFileURL::Get);

			char log[512];
			sprintf(log, "Download asset: %s", filename);
			os::Printer::log(log);
		}
		else
		{
			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				// [bundles].zip
				fileSystem->addFileArchive(filename, false, false);

				if (++m_downloaded >= listBundles.size())
					m_initState = CViewInit::InitScene;
				else
				{
					delete m_getFile;
					m_getFile = NULL;
				}
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
			}
		}
#else

		for (std::string& bundle : listBundles)
		{
			const char *r = bundle.c_str();
#if defined(WINDOWS_STORE)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#elif defined(MACOS)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#else
			fileSystem->addFileArchive(r, false, false);
#endif
		}

		m_initState = CViewInit::InitScene;
#endif
	}
	break;
	case CViewInit::InitScene:
	{
		initScene();
		m_initState = CViewInit::Finished;

#if !defined(__EMSCRIPTEN__)
        CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
#endif
	}
	break;
	case CViewInit::Error:
	{
		// todo nothing with black screen
	}
	break;
	default:
	{
		CScene *scene = context->getScene();
		if (scene != NULL)
			scene->update();
	}
	break;
	}
}

void CViewInit::onRender()
{
	if (m_initState == CViewInit::Finished)
	{
		float blink = 400.0f;
		m_blinkTime = m_blinkTime + getTimeStep();
		if (m_blinkTime < blink)
			m_guiText->setVisible(false);
		else
			m_guiText->setVisible(true);

		if (m_blinkTime > blink * 2.0f)
			m_blinkTime = 0.0f;

		CGraphics2D::getInstance()->render(CContext::getInstance()->getGUICamera());
	}
}

bool CViewInit::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
            if (m_initState == CViewInit::Finished)
                CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
		}
	}

	return false;
}
