#include "pch.h"
#include "SamplePBR.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SamplePBR* app = new SamplePBR();
	getApplication()->registerAppEvent("SamplePBR", app);
}

SamplePBR::SamplePBR()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SamplePBR::~SamplePBR()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void SamplePBR::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SamplePBR::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SamplePBR::onRender()
{
	CViewManager::getInstance()->render();
}

void SamplePBR::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SamplePBR::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SamplePBR::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SamplePBR::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SamplePBR::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SamplePBR::onQuitApp()
{
	// end application
	delete this;
}