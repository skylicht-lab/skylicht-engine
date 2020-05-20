#pragma once

class CButton
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
};