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
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTheme
			{
			public:
				static CTheme* getTheme();

				static void setTheme(CTheme* theme);

				CTheme();

				virtual ~CTheme();

				virtual void drawIcon(const SRect &r, ESystemIcon icon, const SGUIColor& color, bool use32Bit) {}

				virtual void drawDockHintIcon(const SRect &r, EDockHintIcon icon, const SGUIColor& color) {}

				virtual void drawWindowShadow(const SRect& rect) {}

				virtual void drawWindow(const SRect& rect, bool isFocussed) {}

				virtual void drawButtonShadow(const SRect& rect) {}

				virtual void drawButton(const SRect& rect, const SGUIColor& color) {}

				virtual void drawTextbox(const SRect& rect, const SGUIColor& color) {}

				virtual void drawTextboxBorder(const SRect& rect, const SGUIColor& color) {}

				virtual void drawTabButton(const SRect& rect, const SGUIColor& color, const SGUIColor& focusColor, bool focus) {}

				virtual void drawScrollbar(const SRect& rect, const SGUIColor& color, bool isHorizontal) {}
			};
		}
	}
}