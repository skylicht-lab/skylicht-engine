#pragma once

class CButton: public IEventReceiver
{
protected:
	std::string m_label;
	
	CGlyphFont *m_font;
	SFrame* m_frame;
	
	CGUISprite* m_backround;
	CGUIText *m_text;

	SColor m_textColor;

public:
	CButton(CGUIElement *element, SFrame* frame, const char *label, CGlyphFont *font, const SColor& textColor);

	virtual ~CButton();

	virtual bool OnEvent(const SEvent& event);
};