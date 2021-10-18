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
#include "CRulerBar.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CRulerBar::CRulerBar(CBase* parent, bool horizontal) :
				CBase(parent),
				m_isHorizontal(horizontal),
				m_beginOffset(0.0f),
				m_position(0.0f),
				m_pixelPerUnit(5.0f),
				m_textPerUnit(10)
			{
				if (m_isHorizontal)
				{
					setHeight(40.0f);
					dock(EPosition::Top);
				}
				else
				{
					setWidth(40.0f);
					dock(EPosition::Left);
				}

				enableRenderFillRect(true);
				setFillRectColor(CThemeConfig::RulerBG);
			}

			CRulerBar::~CRulerBar()
			{

			}

			void CRulerBar::render()
			{
				CBase::render();

				CRenderer* renderer = CRenderer::getRenderer();

				float size = 0.0f;
				float size1 = 5.0f;		// 1
				float size2 = 10.0f;	// 2
				float size3 = 15.0f;	// 5
				float size4 = 30.0f;	// 10

				int unit = (int)(m_position / m_pixelPerUnit);
				float offset = m_position - unit * m_pixelPerUnit;

				SGUIColor& lineColor = CThemeConfig::RulerLine1;
				wchar_t textValue[64];

				if (m_isHorizontal)
				{
					float x = m_beginOffset - offset;
					float w = width();

					while (x < w)
					{
						size = size1;
						lineColor = CThemeConfig::RulerLine1;

						if (unit % 2 == 0)
						{
							size = size2;
							lineColor = CThemeConfig::RulerLine1;
						}
						if (unit % 5 == 0)
						{
							size = size3;
							lineColor = CThemeConfig::RulerLine2;
						}
						if (unit % 10 == 0)
						{
							size = size4;
							lineColor = CThemeConfig::RulerLine3;
						}

						renderer->drawLineY(x, 0.0f, size, lineColor);

						if (unit % m_textPerUnit == 0)
						{
							SRect r;
							r.X = x + 2.0f;
							r.Y = 15.0f;
							r.Width = 9.0f * m_pixelPerUnit;
							r.Height = 20.0f;
							swprintf(textValue, 64, L"%d", unit);
							renderer->renderText(r, SizeNormal, CThemeConfig::DefaultTextColor, std::wstring(textValue));
						}

						x = x + m_pixelPerUnit;
						unit++;
					}
				}
				else
				{
					float y = m_beginOffset - offset;
					float h = height();

					while (y < h)
					{
						size = size1;
						lineColor = CThemeConfig::RulerLine1;

						if (unit % 2 == 0)
						{
							size = size2;
							lineColor = CThemeConfig::RulerLine1;
						}
						if (unit % 5 == 0)
						{
							size = size3;
							lineColor = CThemeConfig::RulerLine2;
						}
						if (unit % 10 == 0)
						{
							size = size4;
							lineColor = CThemeConfig::RulerLine3;
						}

						renderer->drawLineX(0.0f, y, size, lineColor);

						if (unit % m_textPerUnit == 0)
						{
							SRect r;
							r.X = 15.0f;
							r.Y = y;
							r.Width = 20.0f;
							r.Height = 20.0f;
							swprintf(textValue, 64, L"%d", unit);
							renderer->renderText(r, SizeNormal, CThemeConfig::DefaultTextColor, std::wstring(textValue));
						}

						y = y + m_pixelPerUnit;
						unit++;
					}
				}
			}
		}
	}
}