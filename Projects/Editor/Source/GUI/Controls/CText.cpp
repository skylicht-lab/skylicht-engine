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
#include "CText.h"
#include "GUI/Renderer/CRenderer.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CText::CText(CBase* parent) :
				CBase(parent),
				m_color(CThemeConfig::DefaultTextColor)
			{
				setMouseInputEnabled(false);
				enableClip(true);
			}

			CText::~CText()
			{

			}

			void CText::setString(const std::wstring& string)
			{
				m_string = string;
			}

			SDimension CText::getCharacterPosition(u32 c)
			{
				if (getLength() == 0 || c == 0)
				{
					SDimension p = CRenderer::getRenderer()->measureText(m_fontSize, L" ");
					return SDimension(0.0f, p.Height);
				}

				std::wstring sub;

				if (c > getLengthNoNewLine())
					c = getLengthNoNewLine();

				sub = m_string.substr(0, c);

				return CRenderer::getRenderer()->measureText(m_fontSize, sub);
			}

			u32 CText::getClosestCharacter(const SPoint& point)
			{
				u32 c = 0;
				CRenderer* renderer = CRenderer::getRenderer();

				float minDistance = 5000.0f;
				float x = 0.0f;

				std::wstring s = m_string;
				if (s.length() > 0 && s[s.length() - 1] != '\n')
					s += '\n';

				for (u32 i = 0, n = s.length(); i < n; i++)
				{
					float distance = fabs(x - point.X);

					if (distance < minDistance)
					{
						c = i;
						minDistance = distance;
					}

					float charWidth = renderer->measureCharWidth(m_fontSize, s[i]);
					x = x + charWidth;
				}

				return c;
			}

			void CText::setFontSize(EFontSize size)
			{
				m_fontSize = size;
			}

			void CText::render()
			{
				CRenderer::getRenderer()->renderText(getRenderBounds(), m_fontSize, m_color, m_string);
			}
		}
	}
}