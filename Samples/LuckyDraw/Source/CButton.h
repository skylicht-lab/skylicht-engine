#pragma once

#include <functional>

class CButton : public IEventReceiver
{
protected:
	std::string m_label;

	CGlyphFont *m_font;
	SFrame* m_frame;

	CGUIElement* m_element;
	CGUISprite* m_backround;
	CGUIText *m_text;

	SColor m_textColor;
	bool m_mouseHold;

public:
	std::function<void(void)> OnClick;

public:
	CButton(CGUIElement *element, SFrame* frame, const char *label, CGlyphFont *font, const SColor& textColor);

	virtual ~CButton();

	virtual bool OnEvent(const SEvent& event);
};