#include "pch.h"
#include "CViewHeader.h"
#include "Context/CContext.h"

#include "Graphics2D/CGUIImporter.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIButton.h"

CViewHeader::CViewHeader()
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

	CGUIImporter::loadGUI("SampleGUI/Header.gui", canvas);
	canvas->applyScaleGUI(1.0f);

	UI::CUIContainer* uiContainer = header->addComponent<UI::CUIContainer>();

	UI::CUIBase* btnUserName = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Header/txtUserName"));
	btnUserName->addMotion(UI::EMotionEvent::PointerHover, new UI::CColorMotion(SColor(255, 230, 90, 30)));
	btnUserName->addMotion(UI::EMotionEvent::PointerOut, new UI::CColorMotion());
	btnUserName->addMotion(UI::EMotionEvent::PointerDown, new UI::CAlphaMotion(0.8f))->setTime(0.0f, 0.0f);
	btnUserName->addMotion(UI::EMotionEvent::PointerDown, new UI::CPositionMotion(2.0f, 2.0f, 0.0f))->setTime(0.0f, 50.0f);
	btnUserName->addMotion(UI::EMotionEvent::PointerUp, new UI::CAlphaMotion())->setTime(0.0f, 50.0f);
	btnUserName->addMotion(UI::EMotionEvent::PointerUp, new UI::CPositionMotion());

	UI::CUIBase* btnAvatar = new UI::CUIBase(uiContainer, canvas->getGUIByPath("Canvas/Header/Avatar"));
	btnAvatar->addMotion(UI::EMotionEvent::PointerHover, new UI::CScaleMotion(1.1f, 1.1f, 1.1f));
	btnAvatar->addMotion(UI::EMotionEvent::PointerOut, new UI::CScaleMotion());
	btnAvatar->addMotion(UI::EMotionEvent::PointerDown, new UI::CAlphaMotion(0.8f))->setTime(0.0f, 0.0f);
	btnAvatar->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(1.0f, 1.0f, 1.0f))->setTime(0.0f, 50.0f);
	btnAvatar->addMotion(UI::EMotionEvent::PointerUp, new UI::CAlphaMotion())->setTime(0.0f, 50.0f);
	btnAvatar->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion(1.1f, 1.1f, 1.1f))->setTime(0.0f, 100.0f);
}

void CViewHeader::onDestroy()
{

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
