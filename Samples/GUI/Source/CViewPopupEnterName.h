#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUITextBox.h"

class CViewPopupEnterName : public CView
{
protected:
	CGameObject* m_popup;
	UI::CUITextBox* m_textBox;

public:
	CViewPopupEnterName();

	virtual ~CViewPopupEnterName();

	virtual void onInit();

	virtual void onData();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void close();

	bool onOK();

	void onGUI();

};
