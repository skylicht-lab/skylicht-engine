#include "pch.h"
#include "CViewBase.h"
#include "Context/CContext.h"
#include "Graphics2D/CGUIImporter.h"
#include "CLocalize.h"

CViewBase::CViewBase() :
	m_guiObj(NULL),
	m_canvas(NULL),
	m_uiContainer(NULL)
{

}

CViewBase::~CViewBase()
{
	if (m_guiObj)
		m_guiObj->remove();
}

void CViewBase::loadGUI(const char* gui, CScene* scene)
{
	if (m_guiObj)
		m_guiObj->remove();

	CContext* context = CContext::getInstance();
	if (!scene)
		scene = context->getScene();

	if (!scene)
		return;

	CZone* zone = scene->getZone(0);

	m_guiObj = zone->createEmptyObject();

	m_canvas = m_guiObj->addComponent<CCanvas>();
	m_canvas->OnLocalize = std::bind(&CViewBase::onLocalize, this, std::placeholders::_1);

	m_uiContainer = m_guiObj->addComponent<UI::CUIContainer>();

	float applyScale = 1.0f;

	CGUIImporter::loadGUI(gui, m_canvas);
	m_canvas->applyGUIScale(applyScale);
	m_canvas->updateEntities();

	m_uiContainer->OnPressed = std::bind(&CViewBase::onUIPressed, this, std::placeholders::_1);
}

void CViewBase::onUIPressed(UI::CUIBase* base)
{
	// event when button pressed
}

void CViewBase::onLocalize(CGUIText* text)
{
	// event when load text & localize
	CLocalize* localizeMgr = CLocalize::getInstance();
	ELanguage l = localizeMgr->getLanguage();

	const char* defaultText = text->getText();
	if (defaultText[0] == 0)
		defaultText = text->getDefaultText();

	if (text->getTextId()[0] == 0)
	{
		// no text id -> use default text
		if (text->isStrim())
			text->setTextStrim(defaultText);
		else
			text->setText(defaultText);
	}
	else
	{
		// have text id -> use localize text
		const char* textId = text->getTextId();
		const char* localize = localizeMgr->getString(textId);
		if (localize[0] == 0)
			localize = textId;

		if (text->isStrim())
			text->setTextStrim(localize);
		else
			text->setText(localize);
	}
}

void CViewBase::setThemeButton(UI::CUIButton* btn, float alpha)
{
	btn->addMotion(UI::EMotionEvent::PointerHover, btn->getBackground(), new UI::CAlphaMotion(alpha));
	btn->addMotion(UI::EMotionEvent::PointerOut, btn->getBackground(), new UI::CAlphaMotion());
	btn->addMotion(UI::EMotionEvent::PointerDown, new UI::CPositionMotion(4.0f, 4.0f, 0.0f))->setTime(0.0f, 50.0f);
	btn->addMotion(UI::EMotionEvent::PointerUp, new UI::CPositionMotion())->setTime(0.0f, 100.0f);
}

void CViewBase::setThemeButtonScale(UI::CUIBase* btn)
{
	btn->addMotion(UI::EMotionEvent::PointerHover, new UI::CScaleMotion(1.2f, 1.2f, 1.0f));
	btn->addMotion(UI::EMotionEvent::PointerOut, new UI::CScaleMotion());
	btn->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btn->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
}

void CViewBase::setAlphaScaleButton(UI::CUIButton* btn, float alpha)
{
	btn->addMotion(UI::EMotionEvent::PointerHover, btn->getBackground(), new UI::CAlphaMotion(alpha));
	btn->addMotion(UI::EMotionEvent::PointerOut, btn->getBackground(), new UI::CAlphaMotion());
	btn->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	btn->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
}

void CViewBase::setAlphaScale(UI::CUIBase* ui, float alpha)
{
	ui->addMotion(UI::EMotionEvent::PointerHover, new UI::CAlphaMotion(alpha));
	ui->addMotion(UI::EMotionEvent::PointerOut, new UI::CAlphaMotion());
	ui->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	ui->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
}

void CViewBase::setFadeInOut(UI::CUIBase* ui, float time)
{
	ui->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, time);
	ui->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, time);
}

void CViewBase::setFadeInOutLinear(UI::CUIBase* ui, float time)
{
	ui->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion(0.0f))->setInverse(true)->setTime(0.0f, time)->setEasingFunction(EaseLinear);
	ui->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, time)->setEasingFunction(EaseLinear);
}

void CViewBase::resetHoverButtonMotions(UI::CUIButton* btn, float alpha)
{
	btn->removeMotions(UI::EMotionEvent::PointerHover);
	btn->removeMotions(UI::EMotionEvent::PointerOut);
	btn->addMotion(UI::EMotionEvent::PointerHover, btn->getBackground(), new UI::CAlphaMotion(alpha));
	btn->addMotion(UI::EMotionEvent::PointerOut, btn->getBackground(), new UI::CAlphaMotion());
}
