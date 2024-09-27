#include "pch.h"
#include "CharacterController.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CImguiManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	CharacterController* app = new CharacterController();
	getApplication()->registerAppEvent("CharacterController", app);
}

CharacterController::CharacterController()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
	CImguiManager::createGetInstance();
}

CharacterController::~CharacterController()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
	CImguiManager::releaseInstance();
}

void CharacterController::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void CharacterController::onUpdate()
{
	CViewManager::getInstance()->update();
}

void CharacterController::onRender()
{
	CViewManager::getInstance()->render();
}

void CharacterController::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool CharacterController::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return CViewManager::getInstance()->onBack();
}

void CharacterController::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void CharacterController::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void CharacterController::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void CharacterController::onQuitApp()
{
	// end application
	delete this;
}