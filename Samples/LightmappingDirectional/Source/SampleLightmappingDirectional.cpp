#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleLightmappingDirectional.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmappingDirectional* app = new SampleLightmappingDirectional();
	getApplication()->registerAppEvent("SampleLightmappingDirectional", app);
}

SampleLightmappingDirectional::SampleLightmappingDirectional()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleLightmappingDirectional::~SampleLightmappingDirectional()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleLightmappingDirectional::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleLightmappingDirectional::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleLightmappingDirectional::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleLightmappingDirectional::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleLightmappingDirectional::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void SampleLightmappingDirectional::onResize(int w, int h)
{
	CContext* ctx = CContext::getInstance();
	if (ctx != NULL)
		ctx->resize(w, h);
}

void SampleLightmappingDirectional::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleLightmappingDirectional::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleLightmappingDirectional::onQuitApp()
{
	// end application
	delete this;
}