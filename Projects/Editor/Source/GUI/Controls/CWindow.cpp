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
#include "CDragger.h"
#include "CWindow.h"
#include "CLabel.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CWindow::CWindow(CCanvas* parent, float x, float y, float w, float h) :
				CResizableControl(parent, x, y, w, h)
			{
				m_titleBar = new CDragger(this);
				m_titleBar->setHeight(24);
				m_titleBar->setPadding(SPadding(0, 0, 0, 0));
				m_titleBar->setMargin(SMargin(0, 0, 0, 4));
				m_titleBar->dock(EPosition::Top);

				m_title = new CLabel(m_titleBar);
				m_title->setString(L"Window title");
				m_title->dock(EPosition::Fill);
				m_title->setPadding(SPadding(CThemeConfig::WindowPaddingLeft, CThemeConfig::WindowPaddingTop, 0.0f, 0.0f));
				m_title->setColor(CThemeConfig::WindowTextColor);
			}

			CWindow::~CWindow()
			{

			}

			void CWindow::render()
			{
				bool isFocussed = isOnTop();
				CTheme::getTheme()->drawWindow(getRenderBounds(), isFocussed);
			}

			void CWindow::renderUnder()
			{
				CTheme::getTheme()->drawWindowShadow(getRenderBounds());
			}
		}
	}
}