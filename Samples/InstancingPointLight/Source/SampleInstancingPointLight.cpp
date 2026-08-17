#include "pch.h"
#include "SampleInstancingPointLight.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleInstancingPointLight* app = new SampleInstancingPointLight();
	getApplication()->registerAppEvent("SampleInstancingPointLight", app);
}

SampleInstancingPointLight::SampleInstancingPointLight()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

SampleInstancingPointLight::~SampleInstancingPointLight()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void SampleInstancingPointLight::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleInstancingPointLight::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleInstancingPointLight::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleInstancingPointLight::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleInstancingPointLight::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void SampleInstancingPointLight::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleInstancingPointLight::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleInstancingPointLight::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleInstancingPointLight::onQuitApp()
{
	// end application
	delete this;
}