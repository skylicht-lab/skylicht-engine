#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIProgressBar.h"
#include "UserInterface/CUISlider.h"

class CViewPopupSetting : public CView
{
protected:
	CGameObject* m_popup;

	UI::CUISlider* m_slider;
public:
	CViewPopupSetting();

	virtual ~CViewPopupSetting();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void close();

	void onGUI();

};
