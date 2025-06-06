#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"

class CViewDemo : public CView
{
protected:
	std::vector<CLight*> m_lights;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();
};