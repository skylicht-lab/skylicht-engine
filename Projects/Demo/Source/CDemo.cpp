#include "pch.h"
#include "CDemo.h"

#include "View/CViewInit.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	CDemo *demo = new CDemo();
	getApplication()->registerAppEvent("CDemo", demo);
}

CDemo::CDemo()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
}

CDemo::~CDemo()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();	
}

void CDemo::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void CDemo::onUpdate()
{
	CViewManager::getInstance()->update();
}

void CDemo::onRender()
{
	CViewManager::getInstance()->render();
}

void CDemo::onPostRender()
{
	CViewManager::getInstance()->postRender();
}

void CDemo::onResume()
{
	CViewManager::getInstance()->onResume();
}

void CDemo::onPause()
{
	CViewManager::getInstance()->onPause();
}

void CDemo::onQuitApp()
{
	delete this;
}

bool CDemo::onBack()
{
	return false;
}