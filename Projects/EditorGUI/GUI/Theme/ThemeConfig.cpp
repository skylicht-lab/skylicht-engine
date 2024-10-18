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

#include "pch.h"
#include "ThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			namespace ThemeConfig
			{
				const std::string FontName = "Droid Sans";
				const std::string FontPath = "Editor/Fonts/droidsans.ttf";

				const SGUIColor White = SGUIColor(255, 255, 255, 255);
				const SGUIColor Black = SGUIColor(255, 0, 0, 0);

				const SGUIColor WindowBackgroundColor = SGUIColor(252, 35, 35, 35);
				const SGUIColor WindowInnerColor = SGUIColor(255, 55, 55, 55);
				const SGUIColor DefaultTextColor = SGUIColor(255, 200, 200, 200);
				const SGUIColor DisableTextColor = SGUIColor(100, 200, 200, 200);
				const SGUIColor DefaultIconColor = SGUIColor(255, 215, 215, 215);
				const SGUIColor IconPressColor = SGUIColor(255, 255, 255, 255);
				const SGUIColor TextPressColor = SGUIColor(255, 255, 255, 255);

				const SGUIColor CaretColor = SGUIColor(255, 50, 150, 230);
				const SGUIColor TextHintColor = SGUIColor(255, 110, 110, 110);
				const SGUIColor TextSelectColor = SGUIColor(255, 20, 60, 110);
				const SGUIColor TextSelectUnfocusColor = SGUIColor(255, 60, 60, 60);
				const SGUIColor TextBoxColor = SGUIColor(255, 44, 44, 44);
				const SGUIColor TextBoxBorderColor = SGUIColor(255, 65, 65, 65);
				const SGUIColor TextBoxButtonColor = SGUIColor(255, 110, 110, 110);

				const SGUIColor ButtonColor = SGUIColor(255, 88, 88, 88);
				const SGUIColor ButtonDisableColor = SGUIColor(255, 55, 55, 55);
				const SGUIColor ButtonTextColor = SGUIColor(255, 250, 250, 250);
				const SGUIColor ButtonTextDisableColor = SGUIColor(255, 85, 85, 85);
				const SGUIColor ButtonHoverColor = SGUIColor(255, 110, 110, 110);
				const SGUIColor ButtonPressColor = SGUIColor(255, 80, 120, 180);
				const SGUIColor ButtonFocusColor = SGUIColor(255, 85, 110, 140);

				const SGUIColor SliderColor = ButtonColor;
				const SGUIColor SliderBarColor = SGUIColor(255, 84, 126, 192);

				const SGUIColor SpliterColor = SGUIColor(255, 30, 30, 30);

				const SGUIColor TabTextColor = SGUIColor(255, 145, 145, 145);
				const SGUIColor TabTextFocusColor = ButtonTextColor;
				const SGUIColor TabStripColor = SGUIColor(255, 40, 40, 40);
				const SGUIColor TabButtonColor = SGUIColor(255, 45, 45, 45);
				const SGUIColor TabButtonActiveColor = SGUIColor(255, 65, 65, 65);
				const SGUIColor TabButtonFocusColor = ButtonPressColor;
				const SGUIColor TabCloseButtonHoverColor = ButtonHoverColor;

				const SGUIColor DockHintWindowColor = SGUIColor(150, 90, 90, 180);

				const SGUIColor MenuBarColor = WindowBackgroundColor;
				const SGUIColor MenuItemAcceleratorColor = SGUIColor(100, 250, 250, 250);
				const SGUIColor MenuItemSeparatorColor = WindowInnerColor;

				const SGUIColor ScrollControlBackgroundColor = SGUIColor(100, 20, 20, 20);
				const SGUIColor ScrollBarColor = WindowBackgroundColor;
				const SGUIColor ScrollBarBarColor = ButtonColor;
				const SGUIColor ScrollBarBarHoverColor = ButtonHoverColor;

				const SGUIColor ListItemBackgroundHoverColor = SGUIColor(100, 110, 110, 110);
				const SGUIColor ListItemBackgroundFocusColor = ButtonPressColor;
				const SGUIColor ListItemBackgroundUnfocusColor = SGUIColor(100, 80, 120, 180);

				const SGUIColor CollapsibleColor = SGUIColor(255, 70, 70, 70);
				const SGUIColor CollapsibleInnerColor = WindowInnerColor;

				const SGUIColor OpenSaveDialogTop = SGUIColor(255, 75, 75, 75);
				const SGUIColor OpenSaveDialogBottom = SGUIColor(255, 68, 68, 68);

				const SGUIColor Header = SGUIColor(255, 50, 50, 50);
				const SGUIColor HeaderHover = SGUIColor(255, 60, 60, 60);
				const SGUIColor HeaderBorder = SGUIColor(255, 30, 30, 30);

				const SGUIColor RulerBG = SGUIColor(255, 40, 40, 40);
				const SGUIColor RulerLine1 = SGUIColor(255, 70, 70, 70);
				const SGUIColor RulerLine2 = SGUIColor(255, 100, 100, 100);
				const SGUIColor RulerLine3 = SGUIColor(255, 140, 140, 140);
				const SGUIColor RulerCursor = SGUIColor(255, 80, 120, 180);

				const SGUIColor Timeline = SGUIColor(255, 60, 60, 60);
				const SGUIColor TimelineItemBG = SGUIColor(255, 35, 35, 35);
				const SGUIColor TimelineItemBorder = SGUIColor(255, 10, 10, 10);

				const SGUIColor TimelineBG = SGUIColor(255, 40, 40, 40);
				const SGUIColor TimelineBorder = SGUIColor(255, 60, 60, 60);

				const SGUIColor FolderColor = SGUIColor(255, 190, 160, 95);

				const float TreeIndentationSize = 25.0f;

				float getFontSizePt(EFontSize size)
				{
					switch (size)
					{
					case SizeNormal:
						return 10.0f;
						break;
					case SizeLarge:
						return 20.0f;
					}

					return 10.0f;
				}
			}
		}
	}
}