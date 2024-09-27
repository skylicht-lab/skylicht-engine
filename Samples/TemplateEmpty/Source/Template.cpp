#include "pch.h"
#include "Template.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	Template* app = new Template();
	getApplication()->registerAppEvent("Template", app);
}

Template::Template()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

Template::~Template()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void Template::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void Template::onUpdate()
{
	CViewManager::getInstance()->update();
}

void Template::onRender()
{
	CViewManager::getInstance()->render();
}

void Template::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool Template::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void Template::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void Template::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void Template::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void Template::onQuitApp()
{
	// end application
	delete this;
}