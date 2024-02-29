#include "pch.h"
#include "SampleAnimations.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleAnimations* app = new SampleAnimations();
	getApplication()->registerAppEvent("SampleAnimations", app);
}

SampleAnimations::SampleAnimations()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleAnimations::~SampleAnimations()
{
	CViewManager::releaseInstance();
	CImguiManager::releaseInstance();
	CLightmapper::releaseInstance();
	CContext::releaseInstance();
}

void SampleAnimations::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleAnimations::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleAnimations::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleAnimations::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleAnimations::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleAnimations::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleAnimations::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleAnimations::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleAnimations::onQuitApp()
{
	// end application
	delete this;
}