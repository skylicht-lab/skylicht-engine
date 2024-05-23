#include "pch.h"
#include "SampleOcclusionQuery.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleOcclusionQuery* app = new SampleOcclusionQuery();
	getApplication()->registerAppEvent("SampleOcclusionQuery", app);
}

SampleOcclusionQuery::SampleOcclusionQuery()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleOcclusionQuery::~SampleOcclusionQuery()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void SampleOcclusionQuery::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleOcclusionQuery::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleOcclusionQuery::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleOcclusionQuery::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleOcclusionQuery::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleOcclusionQuery::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleOcclusionQuery::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleOcclusionQuery::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleOcclusionQuery::onQuitApp()
{
	// end application
	delete this;
}