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
#include "CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			std::string CThemeConfig::FontName = "Droid Sans";
			std::string CThemeConfig::FontPath = "Editor/Font/droidsans.ttf";

			SGUIColor CThemeConfig::White = SGUIColor(255, 255, 255, 255);
			SGUIColor CThemeConfig::Black = SGUIColor(255, 0, 0, 0);

			SGUIColor CThemeConfig::WindowBackgroundColor = SGUIColor(252, 35, 35, 35);
			SGUIColor CThemeConfig::WindowInnerColor = SGUIColor(255, 55, 55, 55);
			SGUIColor CThemeConfig::DefaultTextColor = SGUIColor(255, 165, 165, 165);
			SGUIColor CThemeConfig::DefaultIconColor = SGUIColor(255, 215, 215, 215);
			SGUIColor CThemeConfig::IconPressColor = SGUIColor(255, 255, 255, 255);
			SGUIColor CThemeConfig::TextPressColor = SGUIColor(255, 255, 255, 255);

			SGUIColor CThemeConfig::ButtonColor = SGUIColor(255, 88, 88, 88);
			SGUIColor CThemeConfig::ButtonHoverColor = SGUIColor(255, 110, 110, 110);
			SGUIColor CThemeConfig::ButtonPressColor = SGUIColor(255, 80, 120, 180);
			SGUIColor CThemeConfig::ButtonFocusColor = SGUIColor(255, 85, 110, 140);

			float CThemeConfig::getFontSizePt(EFontSize size)
			{
				switch (size)
				{
				case SizeNormal:
					return 10.5f;
					break;
				case SizeLarge:
					return 12.5f;
				}

				return 10.0f;
			}
		}
	}
}