#pragma once

#include "ViewManager/CView.h"
#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIButton.h"

class CViewBase : public CView
{
protected:
	CGameObject* m_guiObj;
	CCanvas* m_canvas;
	UI::CUIContainer* m_uiContainer;

public:
	CViewBase();

	virtual ~CViewBase();

	void loadGUI(const char* gui, CScene* scene);

	void onUIPressed(UI::CUIBase* base);

	void onLocalize(CGUIText* text);

	void setThemeButton(UI::CUIButton* btn, float alpha = 0.8f);

	void setThemeButtonScale(UI::CUIBase* btn);

	void setAlphaScaleButton(UI::CUIButton* btn, float alpha = 0.8f);

	void setAlphaScale(UI::CUIBase* ui, float alpha = 0.8f);

	void setFadeInOut(UI::CUIBase* ui, float time = 250.0f);

	void setFadeInOutLinear(UI::CUIBase* ui, float time = 250.0f);

	void resetHoverButtonMotions(UI::CUIButton* btn, float alpha = 0.8f);
};
