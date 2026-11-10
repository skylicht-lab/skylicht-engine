#pragma once

#include "Utils/CSingleton.h"
#include "CViewLayer.h"

#include "Thread/IMutex.h"

class CViewManager
{
public:
	DECLARE_SINGLETON(CViewManager)

protected:
	std::vector<CViewLayer*> m_viewLayers;

	std::vector<std::function<void()>> m_runInUI;
	std::vector<std::function<void()>> m_willRunUI;
	Skylicht::System::IMutex* m_uiQueueMutex;

public:
	CViewManager();

	virtual ~CViewManager();

	void initViewLayer(int numLayer);

	void releaseAllLayer();

	void runInUI(std::function<void()> func);

	inline int getLayerCount()
	{
		return (int)m_viewLayers.size();
	}

	inline CViewLayer* getLayer(int id)
	{
		return m_viewLayers[id];
	}

	void update();

	void render();

	void postRender();

	void onResume();

	void onPause();

	bool onBack();

	void onData();
};