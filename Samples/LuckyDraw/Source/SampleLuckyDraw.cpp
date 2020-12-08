#include "pch.h"
#include "SampleLuckyDraw.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLuckyDraw* app = new SampleLuckyDraw();
	getApplication()->registerAppEvent("SampleLuckyDraw", app);
}

SampleLuckyDraw::SampleLuckyDraw()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleLuckyDraw::~SampleLuckyDraw()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleLuckyDraw::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleLuckyDraw::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleLuckyDraw::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleLuckyDraw::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleLuckyDraw::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleLuckyDraw::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleLuckyDraw::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleLuckyDraw::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleLuckyDraw::onQuitApp()
{
	// end application
	delete this;
}