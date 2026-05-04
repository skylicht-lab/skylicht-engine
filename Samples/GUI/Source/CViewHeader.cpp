#include "pch.h"
#include "CProfileData.h"
#include "CViewHeader.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewPopupEnterName.h"
#include "CViewPopupSetting.h"

#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIButton.h"

CViewHeader::CViewHeader() :
	m_txtUserName(NULL)
{

}

CViewHeader::~CViewHeader()
{

}

void CViewHeader::onInit()
{
	loadGUI("SampleGUIDemo/Header.gui", NULL);
	m_canvas->setSortDepth(1);

	m_txtUserName = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Header/txtUserName"));
	UI::CUIButton* btnUserName = new UI::CUIButton(m_uiContainer, m_txtUserName);
	setThemeButton(btnUserName);
	btnUserName->OnPressed = [](UI::CUIBase* base)
		{
			CViewManager::getInstance()->getLayer(2)->pushView<CViewPopupEnterName>();
		};

	UI::CUIBase* btnAvatar = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Header/Avatar"));
	setAlphaScale(btnAvatar);

	UI::CUIButton* btnSetting = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Header/BtnSetting"));
	setThemeButtonScale(btnSetting);
	btnSetting->OnPressed = [](UI::CUIBase* base)
		{
			CViewManager::getInstance()->getLayer(2)->pushView<CViewPopupSetting>();
		};
}

void CViewHeader::onDestroy()
{

}

void CViewHeader::onData()
{
	if (m_txtUserName)
		m_txtUserName->setText(CProfileData::getInstance()->Name.c_str());
}

void CViewHeader::onUpdate()
{

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
