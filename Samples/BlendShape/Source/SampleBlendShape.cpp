#include "pch.h"
#include "SampleBlendShape.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleBlendShape* app = new SampleBlendShape();
	getApplication()->registerAppEvent("SampleBlendShape", app);
}

SampleBlendShape::SampleBlendShape()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleBlendShape::~SampleBlendShape()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleBlendShape::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleBlendShape::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleBlendShape::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleBlendShape::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleBlendShape::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleBlendShape::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleBlendShape::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleBlendShape::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleBlendShape::onQuitApp()
{
	// end application
	delete this;
}