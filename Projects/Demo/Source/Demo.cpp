#include "pch.h"
#include "Demo.h"

#include "Material/CShaderManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	Demo *demo = new Demo();
	getApplication()->registerAppEvent("Demo", demo);
}

Demo::Demo():
	m_scene(NULL),
	m_zone(NULL),
	m_camera(NULL),
	m_rendering(NULL)
{

}

Demo::~Demo()
{
	delete m_scene;
	delete m_rendering;
}

io::path Demo::getBuiltInPath(const char *name)
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

void Demo::onInitApp()
{
	io::IFileSystem* fileSystem = getApplication()->getFileSystem();
	fileSystem->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager::getInstance()->initBasicShader();

	initScene();
}

void Demo::initScene()
{
	CBaseApp *app = getApplication();

	m_scene = new CScene();
	m_zone = m_scene->createZone();

	CGameObject *cameraObj = m_zone->createEmptyObject();
	m_camera = cameraObj->addComponent<CCamera>();
	cameraObj->initComponent();

	m_rendering = new CForwardRP();
	m_rendering->initRender(app->getWidth(), app->getHeight());
}

void Demo::onUpdate()
{
	m_scene->update();
}

void Demo::onRender()
{
	m_rendering->render(m_camera, m_zone->getEntityManager(), false);
}

void Demo::onPostRender()
{

}

void Demo::onResume()
{

}

void Demo::onPause()
{

}

void Demo::onQuitApp()
{
	delete this;
}