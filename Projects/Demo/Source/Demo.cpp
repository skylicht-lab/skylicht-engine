#include "pch.h"
#include "Demo.h"

#include "Material/CShaderManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	Demo *demo = new Demo();
	getApplication()->registerAppEvent("Demo", demo);
}

Demo::Demo()
{
}

Demo::~Demo()
{
}

io::path Demo::getBuiltInPath(const char *name)
{
#ifdef __EMSCRIPTEN__
	// Path from ./PrjEmscripten/Projects/MainApp
	std::string assetPath = std::string("../../../Bin/BuiltIn/") + std::string(name);
	return io::path(assetPath.c_str());
#else
	return io::path(name);
#endif
}

void Demo::onInitApp()
{
	io::IFileSystem* fileSystem = getApplication()->getFileSystem();

	// Add built in data
	fileSystem->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	// load basic shader
	CShaderManager::getInstance()->initBasicShader();
}

void Demo::onUpdate()
{

}

void Demo::onRender()
{

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