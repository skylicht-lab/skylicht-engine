#include "pch.h"
#include "SampleGUI.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"
#include "UserInterface/CUIEventManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleGUI* app = new SampleGUI();
	getApplication()->registerAppEvent("SampleGUI", app);
}

SampleGUI::SampleGUI()
{
	CContext::createGetInstance();
	// layer 0: Main GUI
	// layer 1: Header
	// layer 2: Popup
	CViewManager::createGetInstance()->initViewLayer(3);
	CLightmapper::createGetInstance();

	CImguiManager::createGetInstance();
	UI::CUIEventManager::createGetInstance();
}

SampleGUI::~SampleGUI()
{
	UI::CUIEventManager::releaseInstance();
	CImguiManager::releaseInstance();

	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleGUI::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleGUI::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleGUI::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleGUI::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleGUI::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleGUI::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleGUI::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleGUI::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleGUI::onQuitApp()
{
	// end application
	delete this;
}
