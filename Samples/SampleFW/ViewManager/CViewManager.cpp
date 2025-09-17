#include "pch.h"
#include "CViewManager.h"

IMPLEMENT_SINGLETON(CViewManager);

CViewManager::CViewManager()
{
	initViewLayer(1);
}

CViewManager::~CViewManager()
{
	releaseAllLayer();
}

void CViewManager::initViewLayer(int numLayer)
{
	releaseAllLayer();

	for (int i = 0; i < numLayer; i++)
	{
		m_viewLayers.push_back(new CViewLayer());
	}
}

void CViewManager::releaseAllLayer()
{
	for (int i = (int)m_viewLayers.size() - 1; i >= 0; i--)
	{
		delete m_viewLayers[i];
	}
	m_viewLayers.clear();
}

void CViewManager::update()
{
	for (CViewLayer*& layer : m_viewLayers)
	{
		layer->update();
	}
}

void CViewManager::render()
{
	for (CViewLayer*& layer : m_viewLayers)
	{
		layer->render();
	}
}

void CViewManager::postRender()
{
	for (CViewLayer*& layer : m_viewLayers)
	{
		layer->postRender();
	}
}

void CViewManager::onResume()
{
	for (CViewLayer*& layer : m_viewLayers)
	{
		layer->onResume();
	}
}

void CViewManager::onPause()
{
	for (CViewLayer*& layer : m_viewLayers)
	{
		layer->onPause();
	}
}

bool CViewManager::onBack()
{
	bool doDefaultOSBackKey = false;

	for (CViewLayer*& layer : m_viewLayers)
	{
		doDefaultOSBackKey = doDefaultOSBackKey || layer->onBack();
	}

	return doDefaultOSBackKey;
}

void CViewManager::onData()
{
	for (CViewLayer*& layer : m_viewLayers)
	{
		for (int i = 0, n = layer->getViewCount(); i < n; i++)
		{
			layer->getView(i)->onData();
		}
	}
}