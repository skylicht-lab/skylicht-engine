#include "pch.h"
#include "CViewLayer.h"

CViewLayer::CViewLayer() :
	m_view(NULL)
{

}

CViewLayer::~CViewLayer()
{
	destroyAllView();
}

void CViewLayer::update()
{
	int numView = (int)m_willDeleteView.size();
	if (numView > 0)
	{
		for (int i = 0; i < numView; i++)
		{
			m_willDeleteView[i]->onDestroy();
			delete m_willDeleteView[i];
		}

		m_willDeleteView.clear();
	}

	m_view = NULL;

	numView = (int)m_views.size();
	if (numView > 0)
	{
		m_view = m_views[0];
		m_view->onUpdate();
	}
}

void CViewLayer::render()
{
	int numView = (int)m_views.size();
	if (numView > 0)
	{
		if (m_view)
			m_view->onRender();
	}
}

void CViewLayer::postRender()
{
	int numView = (int)m_views.size();
	if (numView > 0)
	{
		if (m_view)
			m_view->onPostRender();
	}
}

void CViewLayer::onResume()
{
	int numView = (int)m_views.size();
	if (numView > 0)
	{
		if (m_view)
			m_view->onResume();
	}
}

void CViewLayer::onPause()
{
	int numView = (int)m_views.size();
	if (numView > 0)
	{
		if (m_view)
			m_view->onPause();
	}
}

bool CViewLayer::onBack()
{
	int numView = (int)m_views.size();
	if (numView > 0)
	{
		if (m_view)
			return m_view->onBack();
	}

	return true;
}


void CViewLayer::popView()
{
	if (m_views.size() > 0)
	{
		m_willDeleteView.push_back(m_views[0]);

		m_views.erase(m_views.begin());
		if (m_views.size() > 0)
			m_views[0]->onActive();
	}
}

void CViewLayer::popAllView()
{
	if (m_views.size() > 0)
	{
		m_willDeleteView.push_back(m_views[0]);

		m_views.erase(m_views.begin());
		if (m_views.size() > 0)
			m_views[0]->onActive();
	}
}

void CViewLayer::popAllViewBefore(CView* s)
{
	while (m_views.size() > 0)
	{
		m_willDeleteView.push_back(m_views[0]);

		m_views.erase(m_views.begin());
	}
}

void CViewLayer::popAllViewTo(CView* s)
{
	while (m_views.size() > 0)
	{
		if (m_views[0] == s)
			return;

		m_willDeleteView.push_back(m_views[0]);

		m_views.erase(m_views.begin());
	}
}

void CViewLayer::removeView(CView* s)
{
	for (int i = 0, n = (int)m_views.size(); i < n; i++)
	{
		if (m_views[i] == s)
		{
			m_willDeleteView.push_back(m_views[i]);

			m_views.erase(m_views.begin() + i);
			return;
		}
	}
}

CView* CViewLayer::getCurrentView()
{
	if (m_views.size() == 0)
		return NULL;

	return m_views[0];
}

CView* CViewLayer::getViewBefore(CView* state)
{
	int numView = (int)m_views.size();
	for (int i = 0; i < numView; i++)
	{
		if (m_views[i] == state)
		{
			if (i < numView - 1)
				return m_views[i + 1];
			else
				return NULL;
		}
	}

	return NULL;
}

void CViewLayer::destroyAllView()
{
	int numView = (int)m_views.size();
	for (int i = 0; i < numView; i++)
	{
		m_views[i]->onDestroy();
		delete m_views[i];
	}

	m_views.clear();
}