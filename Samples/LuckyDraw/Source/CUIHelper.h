#pragma once

#include "UserInterface/CUIButton.h"
#include "UserInterface/CUIListView.h"
#include "UserInterface/CUIContainer.h"

#include "Graphics2D/SpriteFrame/CGlyphFont.h"

class CUIHelper
{
public:
	static UI::CUIButton* addButton(UI::CUIContainer* container, CGUIElement* element, SFrame* frame, const char* label, CGlyphFont* font, const SColor& textColor);

	static UI::CUIButton* addButton(UI::CUIContainer* container, CGUIElement* element, SFrame* frame);

	static UI::CUIListView* addListView(UI::CUIContainer* container, CGUIElement* element);
};