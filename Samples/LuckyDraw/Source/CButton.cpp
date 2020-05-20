#include "pch.h"
#include "SkylichtEngine.h"
#include "CButton.h"

CButton::CButton(CGUIElement *element, SFrame* frame, const char *label, CGlyphFont *font, const SColor& textColor) :
	m_frame(frame),
	m_label(label),
	m_textColor(textColor)
{
	CCanvas *canvas = element->getCanvas();
	m_backround = canvas->createSprite(element, frame);

	m_text = canvas->createText(element, font);
	m_text->setText(m_label.c_str());
	m_text->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
	m_text->setColor(m_textColor);

	CEventManager::getInstance()->registerEvent("CButton", this);
}

CButton::~CButton()
{
	CEventManager::getInstance()->unRegisterEvent(this);
}

bool CButton::OnEvent(const SEvent& event)
{
	return false;
}