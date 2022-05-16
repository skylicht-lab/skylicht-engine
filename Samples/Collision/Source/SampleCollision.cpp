#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleCollision.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"
#include "CImguiManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleCollision* app = new SampleCollision();
	getApplication()->registerAppEvent("SampleCollision", app);
}

SampleCollision::SampleCollision()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();

	CImguiManager::createGetInstance();
}

SampleCollision::~SampleCollision()
{
	CImguiManager::releaseInstance();

	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleCollision::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleCollision::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleCollision::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleCollision::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleCollision::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void SampleCollision::onResize(int w, int h)
{
	// on window size changed
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleCollision::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleCollision::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleCollision::onQuitApp()
{
	// end application
	delete this;
}