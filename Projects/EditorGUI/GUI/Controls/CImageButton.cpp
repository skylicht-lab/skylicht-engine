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
#include "CImageButton.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CImageButton::CImageButton(CBase* parent) :
				CButton(parent),
				m_drawHoverRectangle(true),
				m_hoverRectangleColor(CThemeConfig::ButtonFocusColor),
				m_borderColor(CThemeConfig::ButtonColor)
			{
				setSize(128.0f, 128.0f);
				setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));

				m_label->setHidden(true);
				m_image = new CRawImage(this);
				m_image->dock(EPosition::Fill);
			}

			CImageButton::~CImageButton()
			{

			}

			void CImageButton::renderUnder()
			{
				// do nothing
			}

			void CImageButton::renderFocus()
			{
				CButton::renderFocus();

				if (m_drawHoverRectangle == true)
				{
					SRect renderBounds = getRenderBounds();
					renderBounds.X = renderBounds.X + m_hoverRectangleMargin.Left;
					renderBounds.Y = renderBounds.Y + m_hoverRectangleMargin.Top;
					renderBounds.Width = renderBounds.Width - m_hoverRectangleMargin.Right - m_hoverRectangleMargin.Left;
					renderBounds.Height = renderBounds.Height - m_hoverRectangleMargin.Bottom - m_hoverRectangleMargin.Top;

					if (isHovered())
						CRenderer::getRenderer()->drawBorderRect(renderBounds, m_hoverRectangleColor, true, true, true, true);
					else
						CRenderer::getRenderer()->drawBorderRect(renderBounds, m_borderColor, true, true, true, true);
				}
			}
		}
	}
}