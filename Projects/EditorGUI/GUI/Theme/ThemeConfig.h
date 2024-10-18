/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "GUI/Type.h"
#include "EditorGUIAPI.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			namespace ThemeConfig
			{
				EDITOR_GUI_API extern const std::string FontName;
				EDITOR_GUI_API extern const std::string FontPath;

				EDITOR_GUI_API extern const SGUIColor White;
				EDITOR_GUI_API extern const SGUIColor Black;

				EDITOR_GUI_API extern const SGUIColor WindowBackgroundColor;
				EDITOR_GUI_API extern const SGUIColor WindowInnerColor;
				EDITOR_GUI_API extern const SGUIColor DefaultTextColor;
				EDITOR_GUI_API extern const SGUIColor DisableTextColor;
				EDITOR_GUI_API extern const SGUIColor DefaultIconColor;
				EDITOR_GUI_API extern const SGUIColor IconPressColor;
				EDITOR_GUI_API extern const SGUIColor TextPressColor;

				EDITOR_GUI_API extern const SGUIColor CaretColor;
				EDITOR_GUI_API extern const SGUIColor TextHintColor;
				EDITOR_GUI_API extern const SGUIColor TextSelectColor;
				EDITOR_GUI_API extern const SGUIColor TextSelectUnfocusColor;
				EDITOR_GUI_API extern const SGUIColor TextBoxColor;
				EDITOR_GUI_API extern const SGUIColor TextBoxBorderColor;
				EDITOR_GUI_API extern const SGUIColor TextBoxButtonColor;

				EDITOR_GUI_API extern const SGUIColor ButtonColor;
				EDITOR_GUI_API extern const SGUIColor ButtonDisableColor;
				EDITOR_GUI_API extern const SGUIColor ButtonTextColor;
				EDITOR_GUI_API extern const SGUIColor ButtonTextDisableColor;
				EDITOR_GUI_API extern const SGUIColor ButtonHoverColor;
				EDITOR_GUI_API extern const SGUIColor ButtonPressColor;
				EDITOR_GUI_API extern const SGUIColor ButtonFocusColor;

				EDITOR_GUI_API extern const SGUIColor SliderColor;
				EDITOR_GUI_API extern const SGUIColor SliderBarColor;

				EDITOR_GUI_API extern const SGUIColor TabTextColor;
				EDITOR_GUI_API extern const SGUIColor TabTextFocusColor;
				EDITOR_GUI_API extern const SGUIColor TabStripColor;
				EDITOR_GUI_API extern const SGUIColor TabButtonColor;
				EDITOR_GUI_API extern const SGUIColor TabButtonActiveColor;
				EDITOR_GUI_API extern const SGUIColor TabButtonFocusColor;
				EDITOR_GUI_API extern const SGUIColor TabCloseButtonHoverColor;

				EDITOR_GUI_API extern const SGUIColor SpliterColor;

				EDITOR_GUI_API extern const SGUIColor DockHintWindowColor;

				EDITOR_GUI_API extern const SGUIColor MenuBarColor;
				EDITOR_GUI_API extern const SGUIColor MenuItemAcceleratorColor;
				EDITOR_GUI_API extern const SGUIColor MenuItemSeparatorColor;

				EDITOR_GUI_API extern const SGUIColor ScrollControlBackgroundColor;
				EDITOR_GUI_API extern const SGUIColor ScrollBarColor;
				EDITOR_GUI_API extern const SGUIColor ScrollBarBarColor;
				EDITOR_GUI_API extern const SGUIColor ScrollBarBarHoverColor;

				EDITOR_GUI_API extern const SGUIColor ListItemBackgroundHoverColor;
				EDITOR_GUI_API extern const SGUIColor ListItemBackgroundFocusColor;
				EDITOR_GUI_API extern const SGUIColor ListItemBackgroundUnfocusColor;

				EDITOR_GUI_API extern const SGUIColor CollapsibleColor;
				EDITOR_GUI_API extern const SGUIColor CollapsibleInnerColor;

				EDITOR_GUI_API extern const SGUIColor OpenSaveDialogTop;
				EDITOR_GUI_API extern const SGUIColor OpenSaveDialogBottom;

				EDITOR_GUI_API extern const SGUIColor Header;
				EDITOR_GUI_API extern const SGUIColor HeaderHover;
				EDITOR_GUI_API extern const SGUIColor HeaderBorder;

				EDITOR_GUI_API extern const SGUIColor RulerBG;
				EDITOR_GUI_API extern const SGUIColor RulerLine1;
				EDITOR_GUI_API extern const SGUIColor RulerLine2;
				EDITOR_GUI_API extern const SGUIColor RulerLine3;
				EDITOR_GUI_API extern const SGUIColor RulerCursor;
				EDITOR_GUI_API extern const SGUIColor Timeline;
				EDITOR_GUI_API extern const SGUIColor TimelineItemBG;
				EDITOR_GUI_API extern const SGUIColor TimelineItemBorder;
				EDITOR_GUI_API extern const SGUIColor TimelineBG;
				EDITOR_GUI_API extern const SGUIColor TimelineBorder;

				EDITOR_GUI_API extern const SGUIColor FolderColor;

				EDITOR_GUI_API extern const float TreeIndentationSize;

				EDITOR_GUI_API float getFontSizePt(EFontSize size);
			};
		}
	}
}