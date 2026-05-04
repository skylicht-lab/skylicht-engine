#pragma once

#include "CViewBase.h"
#include "UserInterface/CUIListView.h"
#include "Tween/CTweenFloat.h"

class CViewShop : public CViewBase
{
protected:
	UI::CUIListView* m_listView;
	CTweenFloat* m_scrollTween;

	CGUIElement* m_arrow;
	float m_minArrowX;
	float m_maxArrowX;

	std::vector<float> m_tabSize;

public:
	CViewShop();

	virtual ~CViewShop();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void setCurrentTab(int id);

protected:
	CGUIElement* addShopView(UI::CUIListView* listView, CGUIElement* view);

	void initIAPShopView(CGUIElement* element, const char* itemType);

	void onBtnBottom(UI::CUIBase* btn, int id);

	void updateArrowPosition();
};