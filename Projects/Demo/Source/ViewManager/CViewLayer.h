#ifndef _VIEW_LAYER_H_
#define _VIEW_LAYER_H_

#include "CView.h"

class CViewLayer
{
protected:
	std::vector<CView*> m_views;
	std::vector<CView*> m_willDeleteView;

	CView* m_view;

protected:
	template<class T>
	T* createNewView();

public:
	CViewLayer();

	virtual ~CViewLayer();

	void update();

	void render();

	void postRender();

	void onResume();

	void onPause();

	bool onBack();

	template<class T>
	T* pushView();

	template<class T>
	T* changeView();

	template<class T>
	T* replaceView(CView* oldView);

	void popView();

	void popAllView();

	void popAllViewBefore(CView *s);

	void popAllViewTo(CView *s);

	void removeView(CView *s);

	inline int getViewCount()
	{
		return (int)m_views.size();
	}

	inline CView* getView(int id)
	{
		return m_views[id];
	}

	CView* getCurrentView();

	CView* getViewBefore(CView* state);

	void destroyAllView();
};

template<class T>
T* CViewLayer::createNewView()
{
	T* newView = new T();

	CView *view = dynamic_cast<CView*>(newView);
	if (view == NULL)
	{
		char exceptionInfo[512];
		sprintf(exceptionInfo, "CViewLayer::pushView %s must inherit CView", typeid(T).name());
		os::Printer::log(exceptionInfo);

		delete newView;
		return NULL;
	}

	return newView;
}

template<class T>
T* CViewLayer::pushView()
{
	T* newView = createNewView<T>();
	if (newView == NULL)
		return NULL;

	if (m_views.size() > 0)
		m_views[0]->onDeactive();

	m_views.insert(m_views.begin(), newView);
	newView->onInit();
	return newView;
}

template<class T>
T* CViewLayer::changeView()
{
	if (m_views.size() > 0)
	{
		m_willDeleteView.push_back(m_views[0]);
		m_views[0]->onDestroy();
		m_views.erase(m_views.begin());

		T* newView = createNewView<T>();
		if (newView == NULL)
			return NULL;

		m_views.insert(m_views.begin(), newView);
		newView->onInit();
		return newView;
	}

	return NULL;
}

template<class T>
T* CViewLayer::replaceView(CView* oldView)
{
	for (int i = 0, n = (int)m_views.size(); i < n; i++)
	{
		if (m_views[i] == oldView)
		{
			m_willDeleteView.push_back(m_views[i]);
			m_views[i]->onDestroy();

			m_views.erase(m_views.begin() + i);

			T* newView = createNewView<T>();
			if (newView == NULL)
				return NULL;

			m_views.insert(m_views.begin() + i, newView);
			newView->onInit();
			return newView;
		}
	}

	return NULL;
}

#endif