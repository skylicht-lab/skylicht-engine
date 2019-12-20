#include "pch.h"
#include "Demo.h"

#include "View/CViewInit.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	Demo *demo = new Demo();
	getApplication()->registerAppEvent("Demo", demo);
}

Demo::Demo()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
}

Demo::~Demo()
{
	CContext::releaseInstance();
	CViewManager::releaseInstance();
}

void Demo::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void Demo::onUpdate()
{
	CViewManager::getInstance()->update();
}

void Demo::onRender()
{
	CViewManager::getInstance()->render();
}

void Demo::onPostRender()
{
	CViewManager::getInstance()->postRender();
}

void Demo::onResume()
{
	CViewManager::getInstance()->onResume();
}

void Demo::onPause()
{

}

void Demo::onQuitApp()
{
	delete this;
}