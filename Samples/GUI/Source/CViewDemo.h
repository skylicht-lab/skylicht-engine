#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"

class CViewDemo : public CView
{
protected:

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onGUI();

};
