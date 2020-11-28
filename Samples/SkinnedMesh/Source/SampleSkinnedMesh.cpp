#include "pch.h"
#include "SampleSkinnedMesh.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSkinnedMesh *app = new SampleSkinnedMesh();
	getApplication()->registerAppEvent("SampleSkinnedMesh", app);
}

SampleSkinnedMesh::SampleSkinnedMesh()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleSkinnedMesh::~SampleSkinnedMesh()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleSkinnedMesh::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleSkinnedMesh::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleSkinnedMesh::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleSkinnedMesh::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleSkinnedMesh::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleSkinnedMesh::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleSkinnedMesh::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleSkinnedMesh::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleSkinnedMesh::onQuitApp()
{
	// end application
	delete this;
}