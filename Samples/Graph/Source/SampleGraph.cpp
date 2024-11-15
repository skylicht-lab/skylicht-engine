#include "pch.h"
#include "SampleGraph.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleGraph* app = new SampleGraph();
	getApplication()->registerAppEvent("SampleGraph", app);
}

SampleGraph::SampleGraph()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleGraph::~SampleGraph()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void SampleGraph::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleGraph::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleGraph::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleGraph::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleGraph::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void SampleGraph::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleGraph::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleGraph::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleGraph::onQuitApp()
{
	// end application
	delete this;
}