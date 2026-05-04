#include "pch.h"
#include "CViewPopupEnterName.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CProfileData.h"

#include "Graphics2D/SpriteFrame/CSpriteManager.h"
#include "Utils/CStringImp.h"

#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIButton.h"
#include "UserInterface/CUITextBox.h"

CViewPopupEnterName::CViewPopupEnterName() :
	m_textBox(NULL)
{

}

CViewPopupEnterName::~CViewPopupEnterName()
{

}

void CViewPopupEnterName::onInit()
{
	loadGUI("SamplePurchase/GUI/PopupEnterName.gui", NULL, 1.0f);
	m_canvas->setSortDepth(2);

	UI::CUIBase* bg = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Background"));
	bg->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, 250.0f)->setEasingFunction(EaseLinear);
	bg->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, 250.0f)->setEasingFunction(EaseLinear);

	UI::CUIBase* dialog = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Dialog"));
	dialog->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, 250.0f);
	dialog->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, 250.0f);

	m_textBox = new UI::CUITextBox(m_uiContainer, m_canvas->getGUIByPath("Canvas/Dialog/InputName"));
	m_textBox->setMaxLength(40);

	CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite("SampleGUI/SampleGUI.spritedata");
	SFrame* frame = spriteFrame->getFrameByName("input-activate");
	if (frame && m_textBox->getBackground())
	{
		m_textBox->addMotion(UI::EMotionEvent::Focus, m_textBox->getBackground(), new UI::CFrameMotion(frame))->setTime(0.0f, 50.0f);
		m_textBox->addMotion(UI::EMotionEvent::UnFocus, m_textBox->getBackground(), new UI::CFrameMotion())->setTime(0.0f, 50.0f);
	}

	UI::CUIButton* btnClose = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Dialog/BtnClose"));
	setThemeButtonScale(btnClose);
	btnClose->OnPressed = [&](UI::CUIBase* base)
		{
			close();
		};

	UI::CUIButton* btnCancel = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Dialog/BtnCancel"));
	setThemeButton(btnCancel);
	btnCancel->OnPressed = [&](UI::CUIBase* base)
		{
			close();
		};

	UI::CUIButton* btnOk = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Dialog/BtnOK"));
	setThemeButton(btnOk);
	btnOk->OnPressed = [&](UI::CUIBase* base)
		{
			if (onOK())
				close();
		};

	m_uiContainer->startInMotion();
}

void CViewPopupEnterName::close()
{
	m_uiContainer->startOutMotion();
	m_uiContainer->OnMotionOutFinish = [&]()
		{
			CViewManager::getInstance()->getLayer(1)->removeView(this);
		};
}

bool CViewPopupEnterName::onOK()
{
	wchar_t name[64];
	CStringImp::copy(name, m_textBox->getTextW());
	CStringImp::trim(name);

	if (CStringImp::length(name) > 0)
	{
		CProfileData::getInstance()->Name = name;
		CViewManager::getInstance()->onData();
		return true;
	}
	return false;
}

void CViewPopupEnterName::onData()
{
	m_textBox->setText(CProfileData::getInstance()->Name.c_str());
}

void CViewPopupEnterName::onDestroy()
{

}

void CViewPopupEnterName::onUpdate()
{

}

void CViewPopupEnterName::onRender()
{

}

void CViewPopupEnterName::onGUI()
{

}

void CViewPopupEnterName::onPostRender()
{

}
