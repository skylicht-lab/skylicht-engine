#include "pch.h"
#include "SampleLoadScene.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

#include "PhysicsEngine/CPhysicsEngine.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLoadScene* app = new SampleLoadScene();
	getApplication()->registerAppEvent("SampleLoadScene", app);
}

SampleLoadScene::SampleLoadScene()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();

	Physics::CPhysicsEngine::createGetInstance();
}

SampleLoadScene::~SampleLoadScene()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();

	Physics::CPhysicsEngine::releaseInstance();
}

void SampleLoadScene::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleLoadScene::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleLoadScene::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleLoadScene::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleLoadScene::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void SampleLoadScene::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleLoadScene::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleLoadScene::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleLoadScene::onQuitApp()
{
	// end application
	delete this;
}