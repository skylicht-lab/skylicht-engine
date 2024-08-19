#include "pch.h"
#include "CUIHelper.h"

UI::CUIButton* CUIHelper::addButton(UI::CUIContainer* container, CGUIElement* element, SFrame* frame, const char* label, CGlyphFont* font, const SColor& textColor)
{
	CCanvas* canvas = element->getCanvas();

	CGUISprite* backround = canvas->createSprite(element, frame);
	backround->setName("Background");

	CGUIText* text = canvas->createText(element, font);
	text->setName("Text");
	text->setText(label);
	text->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	text->setColor(textColor);

	UI::CUIButton* btn = new UI::CUIButton(container, element);
	btn->addMotion(UI::EMotionEvent::PointerHover, backround, new UI::CAlphaMotion(0.8f));
	btn->addMotion(UI::EMotionEvent::PointerOut, backround, new UI::CAlphaMotion());
	btn->addMotion(UI::EMotionEvent::PointerDown, new UI::CPositionMotion(2.0f, 2.0f, 0.0f))->setTime(0.0f, 50.0f);
	btn->addMotion(UI::EMotionEvent::PointerUp, new UI::CPositionMotion())->setTime(0.0f, 50.0f);
	return btn;
}

UI::CUIButton* CUIHelper::addButton(UI::CUIContainer* container, CGUIElement* element, SFrame* frame)
{
	CCanvas* canvas = element->getCanvas();

	CGUISprite* backround = canvas->createSprite(element, frame);
	backround->setName("Background");

	UI::CUIButton* btn = new UI::CUIButton(container, element);
	btn->addMotion(UI::EMotionEvent::PointerHover, backround, new UI::CAlphaMotion(0.8f));
	btn->addMotion(UI::EMotionEvent::PointerOut, backround, new UI::CAlphaMotion());
	btn->addMotion(UI::EMotionEvent::PointerDown, new UI::CPositionMotion(2.0f, 2.0f, 0.0f))->setTime(0.0f, 50.0f);
	btn->addMotion(UI::EMotionEvent::PointerUp, new UI::CPositionMotion())->setTime(0.0f, 50.0f);
	return btn;
}

UI::CUIListView* CUIHelper::addListView(UI::CUIContainer* container, CGUIElement* element)
{
	CCanvas* canvas = element->getCanvas();
	core::rectf itemRect = core::rectf(0.0f, 0.0f, element->getWidth(), 50.0f);

	// template item
	CGUIElement* item = canvas->createElement(element, itemRect);
	item->setName("Item");

	return new UI::CUIListView(container, element, item);
}