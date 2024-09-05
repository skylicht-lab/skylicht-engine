#include "pch.h"
#include "SampleSpringBone.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSpringBone* app = new SampleSpringBone();
	getApplication()->registerAppEvent("SampleSpringBone", app);
}

SampleSpringBone::SampleSpringBone()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleSpringBone::~SampleSpringBone()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void SampleSpringBone::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleSpringBone::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleSpringBone::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleSpringBone::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleSpringBone::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleSpringBone::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleSpringBone::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleSpringBone::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleSpringBone::onQuitApp()
{
	// end application
	delete this;
}