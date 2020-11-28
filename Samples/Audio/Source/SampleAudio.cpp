#include "pch.h"
#include "SampleAudio.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleAudio *app = new SampleAudio();
	getApplication()->registerAppEvent("SampleAudio", app);
}

SampleAudio::SampleAudio()
{
	SkylichtAudio::initSkylichtAudio();
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleAudio::~SampleAudio()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	SkylichtAudio::releaseSkylichtAudio();
}

void SampleAudio::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleAudio::onUpdate()
{
	SkylichtAudio::updateSkylichtAudio();

	CViewManager::getInstance()->update();
}

void SampleAudio::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleAudio::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleAudio::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleAudio::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleAudio::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleAudio::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleAudio::onQuitApp()
{
	// end application
	delete this;
}