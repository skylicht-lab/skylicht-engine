#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"

class CViewPopupEnterName : public CView
{
protected:
	CGameObject* m_popup;

public:
	CViewPopupEnterName();

	virtual ~CViewPopupEnterName();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void close();

	void onGUI();

};
