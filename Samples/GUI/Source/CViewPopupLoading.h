#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIProgressBar.h"

class CViewPopupLoading : public CView
{
protected:
	CGameObject* m_popup;
	UI::CUIProgressBar* m_progressBar;

public:
	CViewPopupLoading();

	virtual ~CViewPopupLoading();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void close();

	void onGUI();

};
