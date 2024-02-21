#include "pch.h"
#include "SkylichtEngine.h"
#include "SamplePhysics.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

#include "PhysicsEngine/CPhysicsEngine.h"

void installApplication(const std::vector<std::string>& argv)
{
	SamplePhysics* app = new SamplePhysics();
	getApplication()->registerAppEvent("SamplePhysics", app);
}

SamplePhysics::SamplePhysics()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();

	Physics::CPhysicsEngine::createGetInstance();
}

SamplePhysics::~SamplePhysics()
{
	Physics::CPhysicsEngine::releaseInstance();

	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SamplePhysics::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SamplePhysics::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SamplePhysics::onRender()
{
	CViewManager::getInstance()->render();
}

void SamplePhysics::onPostRender()
{
	CViewManager::getInstance()->postRender();
}

bool SamplePhysics::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SamplePhysics::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SamplePhysics::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SamplePhysics::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SamplePhysics::onQuitApp()
{
	// end application
	delete this;
}