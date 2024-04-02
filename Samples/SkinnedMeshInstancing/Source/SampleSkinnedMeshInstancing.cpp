#include "pch.h"
#include "SampleSkinnedMeshInstancing.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSkinnedMeshInstancing *app = new SampleSkinnedMeshInstancing();
	getApplication()->registerAppEvent("SampleSkinnedMeshInstancing", app);
}

SampleSkinnedMeshInstancing::SampleSkinnedMeshInstancing()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleSkinnedMeshInstancing::~SampleSkinnedMeshInstancing()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleSkinnedMeshInstancing::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleSkinnedMeshInstancing::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleSkinnedMeshInstancing::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleSkinnedMeshInstancing::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleSkinnedMeshInstancing::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleSkinnedMeshInstancing::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleSkinnedMeshInstancing::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleSkinnedMeshInstancing::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleSkinnedMeshInstancing::onQuitApp()
{
	// end application
	delete this;
}