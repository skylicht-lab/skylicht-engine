#include "pch.h"
#include "CProfileData.h"
#include "CViewHeader.h"
#include "CViewShop.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewPopupEnterName.h"

#include "Utils/CStringImp.h"
#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIButton.h"

CViewHeader::CViewHeader() :
	m_txtUserName(NULL),
	m_txtGold(NULL),
	m_txtDiamond(NULL),
	m_gold(0.0f),
	m_diamond(0.0f),
	m_updateCurrency(false)
{

}

CViewHeader::~CViewHeader()
{

}

void CViewHeader::onInit()
{
	loadGUI("SamplePurchase/GUI/Header.gui", NULL, 1.0f);
	m_canvas->setSortDepth(1);

	m_txtUserName = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Header/TxtUserName"));
	UI::CUIButton* btnUserName = new UI::CUIButton(m_uiContainer, m_txtUserName);
	setThemeButton(btnUserName);
	btnUserName->OnPressed = [](UI::CUIBase* base)
		{
			CViewManager::getInstance()->getLayer(2)->pushView<CViewPopupEnterName>();
		};

	UI::CUIBase* btnAvatar = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Header/Avatar"));
	btnAvatar->addMotion(UI::EMotionEvent::PointerHover, new UI::CAlphaMotion(0.8f));
	btnAvatar->addMotion(UI::EMotionEvent::PointerOut, new UI::CAlphaMotion());
	btnAvatar->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btnAvatar->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);

	m_txtGold = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Gold/Text"));
	UI::CUIButton* btnGold = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Gold"));
	setThemeButton(btnGold);
	btnGold->OnPressed = [](UI::CUIBase* base)
		{
			CView* view = CViewManager::getInstance()->getLayer(0)->getCurrentView();
			CViewShop* shopView = dynamic_cast<CViewShop*>(view);
			if (shopView)
				shopView->setCurrentTab(1);
		};

	m_txtDiamond = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Diamond/Text"));
	UI::CUIButton* btnDiamond = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Diamond"));
	setThemeButton(btnDiamond);
	btnDiamond->OnPressed = [](UI::CUIBase* base)
		{
			CView* view = CViewManager::getInstance()->getLayer(0)->getCurrentView();
			CViewShop* shopView = dynamic_cast<CViewShop*>(view);
			if (shopView)
				shopView->setCurrentTab(0);
		};

	updateConsumer(false);
}

void CViewHeader::onDestroy()
{

}

void CViewHeader::onData()
{
	if (m_txtUserName)
		m_txtUserName->setText(CProfileData::getInstance()->Name.c_str());

	updateConsumer(true);
}

void CViewHeader::onUpdate()
{
	if (m_updateCurrency)
	{
		float gold = (float)CProfileData::getInstance()->Gold;
		float diamond = (float)CProfileData::getInstance()->Diamond;

		float ts = getTimeScale() * 0.2f;

		float g = gold - m_gold;
		float d = diamond - m_diamond;

		m_gold = g <= 50 ? gold : m_gold + g * ts;
		m_diamond = d <= 50 ? diamond : m_diamond + d * ts;

		if (m_txtGold)
			m_txtGold->setText(CStringImp::formatThousand((int)m_gold, true, true).c_str());

		if (m_txtDiamond)
			m_txtDiamond->setText(CStringImp::formatThousand((int)m_diamond, true, true).c_str());

		if (m_gold == gold && m_diamond == diamond)
		{
			m_updateCurrency = false;
		}
	}
}

void CViewHeader::onRender()
{

}

void CViewHeader::onGUI()
{

}

void CViewHeader::onPostRender()
{

}

void CViewHeader::updateConsumer(bool playAnim)
{
	if (playAnim)
		m_updateCurrency = true;
	else
	{
		m_gold = (float)CProfileData::getInstance()->Gold;
		m_diamond = (float)CProfileData::getInstance()->Diamond;

		if (m_txtGold)
			m_txtGold->setText(CStringImp::formatThousand((int)m_gold, true, true).c_str());

		if (m_txtDiamond)
			m_txtDiamond->setText(CStringImp::formatThousand((int)m_diamond, true, true).c_str());
	}
}
