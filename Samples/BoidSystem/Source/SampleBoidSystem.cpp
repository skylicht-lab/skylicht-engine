#include "pch.h"
#include "SampleBoidSystem.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleBoidSystem *app = new SampleBoidSystem();
	getApplication()->registerAppEvent("SampleBoidSystem", app);
}

SampleBoidSystem::SampleBoidSystem()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleBoidSystem::~SampleBoidSystem()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleBoidSystem::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleBoidSystem::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleBoidSystem::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleBoidSystem::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleBoidSystem::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleBoidSystem::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleBoidSystem::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleBoidSystem::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleBoidSystem::onQuitApp()
{
	// end application
	delete this;
}