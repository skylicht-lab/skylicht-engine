#include "pch.h"
#include "SampleLightmappingVertex.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"
#include "CViewBakeLightmap.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmappingVertex *app = new SampleLightmappingVertex();
	getApplication()->registerAppEvent("SampleLightmappingVertex", app);
}

SampleLightmappingVertex::SampleLightmappingVertex()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleLightmappingVertex::~SampleLightmappingVertex()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleLightmappingVertex::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleLightmappingVertex::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleLightmappingVertex::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleLightmappingVertex::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleLightmappingVertex::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleLightmappingVertex::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleLightmappingVertex::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleLightmappingVertex::onQuitApp()
{
	// end application
	delete this;
}