#include "pch.h"
#include "SkylichtEngine.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "SampleLightmapping.h"
#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmapping *app = new SampleLightmapping();
	getApplication()->registerAppEvent("SampleLightmapping", app);
}

SampleLightmapping::SampleLightmapping()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleLightmapping::~SampleLightmapping()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleLightmapping::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleLightmapping::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleLightmapping::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleLightmapping::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleLightmapping::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleLightmapping::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleLightmapping::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleLightmapping::onQuitApp()
{
	// end application
	delete this;
}