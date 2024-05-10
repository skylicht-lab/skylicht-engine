#pragma once

#include "ViewManager/CView.h"
#include "SkinnedInstancing/CRenderSkinnedInstancing.h"

class CViewDemo : public CView
{
protected:
	std::vector<CRenderSkinnedInstancing*> m_instancings;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void followRandomEntity();
};