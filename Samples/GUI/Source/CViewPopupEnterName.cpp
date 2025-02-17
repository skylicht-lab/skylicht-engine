#include "pch.h"
#include "CViewPopupEnterName.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CProfileData.h"

#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIButton.h"
#include "UserInterface/CUITextBox.h"

CViewPopupEnterName::CViewPopupEnterName() :
	m_popup(NULL),
	m_textBox(NULL)
{

}

CViewPopupEnterName::~CViewPopupEnterName()
{

}

void CViewPopupEnterName::onInit()
{
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	m_popup = zone->createEmptyObject();
	CCanvas* canvas = m_popup->addComponent<CCanvas>();

	CGUIImporter::loadGUI("SampleGUIDemo/PopupEnterName.gui", canvas);
	canvas->applyScaleGUI(1.0f);
	canvas->setSortDepth(2);

	UI::CUIContainer* uiContainer = m_popup->addComponent<UI::CUIContainer>();

	UI::CUIBase* bg = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Background"));
	bg->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, 250.0f)->setEasingFunction(EaseLinear);
	bg->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, 250.0f)->setEasingFunction(EaseLinear);

	UI::CUIBase* dialog = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Dialog"));
	dialog->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, 250.0f);
	dialog->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, 250.0f);

	m_textBox = new UI::CUITextBox(uiContainer, canvas->getGUIByPath("Canvas/Dialog/InputName"));

	CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite("SampleGUI/SampleGUI.spritedata");
	SFrame* frame = spriteFrame->getFrameByName("input-activate");
	if (frame && m_textBox->getBackground())
	{
		m_textBox->addMotion(UI::EMotionEvent::Focus, m_textBox->getBackground(), new UI::CFrameMotion(frame))->setTime(0.0f, 50.0f);
		m_textBox->addMotion(UI::EMotionEvent::UnFocus, m_textBox->getBackground(), new UI::CFrameMotion())->setTime(0.0f, 50.0f);
	}

	UI::CUIBase* btnClose = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Dialog/btnClose"));
	btnClose->addMotion(UI::EMotionEvent::PointerHover, new UI::CScaleMotion(1.2f, 1.2f, 1.0f));
	btnClose->addMotion(UI::EMotionEvent::PointerOut, new UI::CScaleMotion());
	btnClose->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btnClose->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
	btnClose->OnPressed = [&](UI::CUIBase* base)
		{
			close();
		};

	UI::CUIButton* btnCancel = new UI::CUIButton(uiContainer, canvas->getGUIByPath("Canvas/Dialog/btnCancel"));
	btnCancel->addMotion(UI::EMotionEvent::PointerHover, btnCancel->getBackground(), new UI::CAlphaMotion(0.8f));
	btnCancel->addMotion(UI::EMotionEvent::PointerOut, btnCancel->getBackground(), new UI::CAlphaMotion());
	btnCancel->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btnCancel->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
	btnCancel->OnPressed = [&](UI::CUIBase* base)
		{
			close();
		};

	UI::CUIButton* btnOk = new UI::CUIButton(uiContainer, canvas->getGUIByPath("Canvas/Dialog/btnOK"));
	btnOk->addMotion(UI::EMotionEvent::PointerHover, btnOk->getBackground(), new UI::CAlphaMotion(0.8f));
	btnOk->addMotion(UI::EMotionEvent::PointerOut, btnOk->getBackground(), new UI::CAlphaMotion());
	btnOk->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btnOk->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
	btnOk->OnPressed = [&](UI::CUIBase* base)
		{
			if (onOK())
				close();
		};

	uiContainer->startInMotion();
}

void CViewPopupEnterName::close()
{
	UI::CUIContainer* uiContainer = m_popup->getComponent<UI::CUIContainer>();
	uiContainer->startOutMotion();
	uiContainer->OnMotionOutFinish = [&]()
		{
			m_popup->remove();
			CViewManager::getInstance()->getLayer(2)->removeView(this);
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
