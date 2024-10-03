#include "pch.h"
#include "SampleSpine2D.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSpine2D* app = new SampleSpine2D();
	getApplication()->registerAppEvent("SampleSpine2D", app);
}

SampleSpine2D::SampleSpine2D()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleSpine2D::~SampleSpine2D()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void SampleSpine2D::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleSpine2D::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleSpine2D::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleSpine2D::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleSpine2D::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void SampleSpine2D::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleSpine2D::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleSpine2D::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleSpine2D::onQuitApp()
{
	// end application
	delete this;
}