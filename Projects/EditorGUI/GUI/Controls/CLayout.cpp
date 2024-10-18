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
#include "CLayout.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/CGUIContext.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CLayout::CLayout(CBase* parent) :
				CBase(parent),
				m_childPadding(0.0f),
				m_renderHover(false),
				m_hoverColor(CThemeConfig::ButtonPressColor)
			{

			}

			CLayout::~CLayout()
			{

			}

			void CLayout::renderUnder()
			{
				CBase::renderUnder();

				if (m_renderHover && (isHovered() || isChild(CGUIContext::HoveredControl, true)))
				{
					SRect bound = getRenderBounds();
					bound.X = 2.0f;
					bound.Y = 1.0f;
					bound.Height = 18.0f;
					bound.Width = bound.Width - 4.0f;
					CRenderer::getRenderer()->drawFillRect(bound, m_hoverColor);
				}
			}
		}
	}
}