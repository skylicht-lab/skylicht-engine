#pragma once

#include "ViewManager/CView.h"
#include "OcclusionQuery/COcclusionQuery.h"

class CViewDemo : public CView
{
protected:
	COcclusionQuery* m_cubeOcclusionQuery;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void onGUI();
};