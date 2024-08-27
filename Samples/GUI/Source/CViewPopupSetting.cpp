#include "pch.h"
#include "CViewPopupSetting.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "Tween/CTweenManager.h"

CViewPopupSetting::CViewPopupSetting() :
	m_popup(NULL)
{

}

CViewPopupSetting::~CViewPopupSetting()
{

}

void CViewPopupSetting::onInit()
{
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	m_popup = zone->createEmptyObject();
	CCanvas* canvas = m_popup->addComponent<CCanvas>();

	CGUIImporter::loadGUI("SampleGUIDemo/Setting.gui", canvas);
	canvas->applyScaleGUI(1.0f);
	canvas->setSortDepth(2);

	UI::CUIContainer* uiContainer = m_popup->addComponent<UI::CUIContainer>();

	UI::CUIBase* bg = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Container/Background"));
	bg->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, 250.0f)->setEasingFunction(EaseLinear);
	bg->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, 250.0f)->setEasingFunction(EaseLinear);

	UI::CUIBase* dialog = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Container/Dialog"));
	dialog->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, 250.0f);
	dialog->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, 250.0f);

	UI::CUIBase* btnClose = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Container/Dialog/BtnClose"));
	btnClose->addMotion(UI::EMotionEvent::PointerHover, new UI::CScaleMotion(1.2f, 1.2f, 1.0f));
	btnClose->addMotion(UI::EMotionEvent::PointerOut, new UI::CScaleMotion());
	btnClose->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btnClose->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
	btnClose->OnPressed = [&](UI::CUIBase* base)
		{
			close();
		};

	uiContainer->startInMotion();
}

void CViewPopupSetting::close()
{
	UI::CUIContainer* uiContainer = m_popup->getComponent<UI::CUIContainer>();
	uiContainer->startOutMotion();
	uiContainer->OnMotionOutFinish = [&]()
		{
			m_popup->remove();
			CViewManager::getInstance()->getLayer(2)->removeView(this);
		};
}

void CViewPopupSetting::onDestroy()
{

}

void CViewPopupSetting::onUpdate()
{

}

void CViewPopupSetting::onRender()
{

}

void CViewPopupSetting::onGUI()
{

}

void CViewPopupSetting::onPostRender()
{

}
