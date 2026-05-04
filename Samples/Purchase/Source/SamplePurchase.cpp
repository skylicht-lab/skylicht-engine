#include "pch.h"
#include "SamplePurchase.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "UserInterface/CUIEventManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"
#include "CProfileData.h"
#include "CShopConfig.h"
#include "CLocalize.h"
#include "CPurchase.h"

void installApplication(const std::vector<std::string>& argv)
{
	CSamplePurchase* app = new CSamplePurchase();
	getApplication()->registerAppEvent("CSamplePurchase", app);
}

CSamplePurchase::CSamplePurchase()
{
	CContext::createGetInstance();
	CProfileData::createGetInstance();
	CPurchase::createGetInstance();

	// layer 0: Main GUI
	// layer 1: Header
	// layer 2: Popup
	CViewManager::createGetInstance()->initViewLayer(3);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();

	UI::CUIEventManager::createGetInstance();

	CLocalize::createGetInstance();
	CShopConfig::createGetInstance();
}

CSamplePurchase::~CSamplePurchase()
{
	CShopConfig::releaseInstance();
	CLocalize::releaseInstance();
	CPurchase::releaseInstance();

	UI::CUIEventManager::releaseInstance();

	CViewManager::releaseInstance();
	CProfileData::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void CSamplePurchase::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void CSamplePurchase::onUpdate()
{
	CViewManager::getInstance()->update();
}

void CSamplePurchase::onRender()
{
	CViewManager::getInstance()->render();
}

void CSamplePurchase::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool CSamplePurchase::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void CSamplePurchase::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void CSamplePurchase::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void CSamplePurchase::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void CSamplePurchase::onQuitApp()
{
	// end application
	delete this;
}