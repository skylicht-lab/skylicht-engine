#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"
#include "ViewManager/CViewManager.h"

#include "Context/CContext.h"

#include "Material/CShaderManager.h"

#include "Camera/CEditorCamera.h"
#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char *name)
{
#ifdef __EMSCRIPTEN__
	// Path from ./PrjEmscripten/Projects/MainApp
	std::string assetPath = std::string("../../../Bin/BuiltIn/") + std::string(name);
	return io::path(assetPath.c_str());
#elif defined(WINDOWS_STORE)
	std::string assetPath = std::string("Assets\\") + std::string(name);
	return io::path(assetPath.c_str());
#else
	return io::path(name);
#endif
}

void CViewInit::onInit()
{
	getApplication()->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager::getInstance()->initBasicShader();
}

void CViewInit::initScene()
{
	CBaseApp *app = getApplication();

	CScene *scene = CContext::getInstance()->initScene();
	CZone *zone = scene->createZone();

	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>();

	CCamera *camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(3.0f, 3.0f, 3.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	ITexture *skyDomeTexture = getVideoDriver()->getTexture("Demo/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	zone->createEmptyObject()->addComponent<CGridPlane>();

	CContext *context = CContext::getInstance();
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
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
#ifdef __EMSCRIPTEN__
		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL("CDemo.zip", "CDemo.zip");
			m_getFile->download(CGetFileURL::Get);
		}
		else
		{
			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				// add demo.zip after it downloaded
				fileSystem->addFileArchive("CDemo.zip", false, false);
				m_initState = CViewInit::InitScene;
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
			}
		}
#else

#if defined(WINDOWS_STORE)
		fileSystem->addFileArchive(getBuiltInPath("Demo.zip"), false, false);
#else
		fileSystem->addFileArchive("Demo.zip", false, false);
#endif	

		m_initState = CViewInit::InitScene;
#endif
	}
	break;
	case CViewInit::InitScene:
	{
		initScene();
		m_initState = CViewInit::Finished;
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

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
	break;
	}
}

void CViewInit::onRender()
{

}