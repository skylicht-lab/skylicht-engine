#include "pch.h"
#include "TankScene.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"
#include "CViewBakeLightmap.h"

void installApplication(const std::vector<std::string>& argv)
{
	TankScene *app = new TankScene();
	getApplication()->registerAppEvent("TankScene", app);

	if (argv.size() >= 4)
	{
		CViewBakeLightmap::s_numLightBound = atoi(argv[3].c_str());
	}
}

TankScene::TankScene()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

TankScene::~TankScene()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void TankScene::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void TankScene::onUpdate()
{
	CViewManager::getInstance()->update();
}

void TankScene::onRender()
{
	CViewManager::getInstance()->render();
}

void TankScene::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool TankScene::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void TankScene::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void TankScene::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void TankScene::onQuitApp()
{
	// end application
	delete this;
}