#include "pch.h"
#include "CViewManager.h"

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
	for (CViewLayer *&layer : m_viewLayers)
	{
		delete layer;
	}
	m_viewLayers.clear();
}

void CViewManager::update()
{
	for (CViewLayer *&layer : m_viewLayers)
	{
		layer->update();
	}
}

void CViewManager::render()
{
	for (CViewLayer *&layer : m_viewLayers)
	{
		layer->render();
	}
}

void CViewManager::postRender()
{
	for (CViewLayer *&layer : m_viewLayers)
	{
		layer->postRender();
	}
}

void CViewManager::onResume()
{
	for (CViewLayer *&layer : m_viewLayers)
	{
		layer->onResume();
	}
}

void CViewManager::onPause()
{
	for (CViewLayer *&layer : m_viewLayers)
	{
		layer->onPause();
	}
}

bool CViewManager::onBack()
{
	return true;
}