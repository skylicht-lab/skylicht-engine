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
#include "CScrollBarBar.h"
#include "CScrollBar.h"

#include "GUI/Theme/CTheme.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CScrollBarBar::CScrollBarBar(CBase *parent, bool horizontal) :
				CDragger(parent),
				m_isHoriontal(horizontal)
			{
				setTarget(this);
				setSize(6.0f, 6.0f);

				enableRenderFillRect(true);
				setFillRectColor(SGUIColor(255, 255, 255, 255));
			}

			CScrollBarBar::~CScrollBarBar()
			{

			}

			void CScrollBarBar::render()
			{
				SGUIColor c = CThemeConfig::ScrollBarBarColor;
				if (isHovered())
					c = CThemeConfig::ScrollBarBarHoverColor;

				CTheme::getTheme()->drawScrollbar(getRenderBounds(), c, m_isHoriontal);
			}

			void CScrollBarBar::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				CDragger::onMouseMoved(x, y, deltaX, deltaY);

				if (!m_disabled && m_pressed && OnBarMoved != nullptr)
				{
					OnBarMoved(this);
				}
			}
		}
	}
}