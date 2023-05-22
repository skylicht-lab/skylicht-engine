#include "pch.h"
#include "SampleSponza.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "Lightmapper/CLightmapper.h"
#include "CImguiManager.h"

using namespace Lightmapper;

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSponza* app = new SampleSponza();
	getApplication()->registerAppEvent("SampleSponza", app);
}

SampleSponza::SampleSponza()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleSponza::~SampleSponza()
{
	CLightmapper::releaseInstance();
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CImguiManager::releaseInstance();
}

void SampleSponza::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleSponza::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleSponza::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleSponza::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleSponza::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleSponza::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleSponza::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleSponza::onResize(int w, int h)
{
	CContext* ctx = CContext::getInstance();
	if (ctx != NULL)
		ctx->resize(w, h);
}

void SampleSponza::onQuitApp()
{
	// end application
	delete this;
}
