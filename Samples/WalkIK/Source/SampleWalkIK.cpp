#include "pch.h"
#include "SampleWalkIK.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleWalkIK* app = new SampleWalkIK();
	getApplication()->registerAppEvent("SampleWalkIK", app);
}

SampleWalkIK::SampleWalkIK()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleWalkIK::~SampleWalkIK()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void SampleWalkIK::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleWalkIK::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleWalkIK::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleWalkIK::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleWalkIK::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleWalkIK::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleWalkIK::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleWalkIK::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleWalkIK::onQuitApp()
{
	// end application
	delete this;
}