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
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	CGameObject* header = zone->createEmptyObject();
	CCanvas* canvas = header->addComponent<CCanvas>();

	CGUIImporter::loadGUI("SampleGUIDemo/Header.gui", canvas);
	canvas->applyScaleGUI(1.0f);
	canvas->setSortDepth(1);

	UI::CUIContainer* uiContainer = header->addComponent<UI::CUIContainer>();

	m_txtUserName = dynamic_cast<CGUIText*>(canvas->getGUIByPath("Canvas/Header/txtUserName"));
	UI::CUIBase* btnUserName = new UI::CUIBase(uiContainer, m_txtUserName);
	btnUserName->addMotion(UI::EMotionEvent::PointerHover, new UI::CColorMotion(SColor(255, 230, 90, 30)));
	btnUserName->addMotion(UI::EMotionEvent::PointerOut, new UI::CColorMotion());
	btnUserName->addMotion(UI::EMotionEvent::PointerDown, new UI::CAlphaMotion(0.8f))->setTime(0.0f, 0.0f);
	btnUserName->addMotion(UI::EMotionEvent::PointerDown, new UI::CPositionMotion(2.0f, 2.0f, 0.0f))->setTime(0.0f, 50.0f);
	btnUserName->addMotion(UI::EMotionEvent::PointerUp, new UI::CAlphaMotion())->setTime(0.0f, 50.0f);
	btnUserName->addMotion(UI::EMotionEvent::PointerUp, new UI::CPositionMotion());
	btnUserName->OnPressed = [](UI::CUIBase* base)
		{
			CViewManager::getInstance()->getLayer(2)->pushView<CViewPopupEnterName>();
		};

	UI::CUIBase* btnAvatar = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Header/Avatar"));
	btnAvatar->addMotion(UI::EMotionEvent::PointerHover, new UI::CAlphaMotion(0.8f));
	btnAvatar->addMotion(UI::EMotionEvent::PointerOut, new UI::CAlphaMotion());
	btnAvatar->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btnAvatar->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);

	UI::CUIButton* btnSetting = new UI::CUIButton(uiContainer, canvas->getGUIByPath("Canvas/Header/BtnSetting"));
	btnSetting->addMotion(UI::EMotionEvent::PointerHover, btnSetting->getBackground(), new UI::CVisibleMotion(true));
	btnSetting->addMotion(UI::EMotionEvent::PointerOut, btnSetting->getBackground(), new UI::CVisibleMotion());
	btnSetting->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btnSetting->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
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
