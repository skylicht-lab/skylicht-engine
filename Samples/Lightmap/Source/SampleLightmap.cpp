#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleLightmap.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmap* app = new SampleLightmap();
	getApplication()->registerAppEvent("SampleLightmap", app);
}

SampleLightmap::SampleLightmap()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleLightmap::~SampleLightmap()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleLightmap::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleLightmap::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleLightmap::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleLightmap::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleLightmap::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void SampleLightmap::onResize(int w, int h)
{
	CContext* ctx = CContext::getInstance();
	if (ctx != NULL)
		ctx->resize(w, h);
}

void SampleLightmap::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleLightmap::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleLightmap::onQuitApp()
{
	// end application
	delete this;
}