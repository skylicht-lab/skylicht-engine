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
			CWindow::CWindow(CBase* parent, float x, float y, float w, float h) :
				CResizableControl(parent, x, y, w, h)
			{
				enableClip(true);

				m_titleBar = new CDragger(this);
				m_titleBar->setHeight(22.0f);
				m_titleBar->setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));
				m_titleBar->setMargin(SMargin(0.0f, 0.0f, 0.0f, 4.0f));
				m_titleBar->dock(EPosition::Top);

				m_icon = new CIcon(m_titleBar, ESystemIcon::Window, false);
				m_icon->dock(EPosition::Left);
				m_icon->setMargin(SPadding(0.0f, 0.0f, 0.0f, 0.0f));

				m_title = new CLabel(m_titleBar);
				m_title->setString(L"Window title");
				m_title->dock(EPosition::Fill);
				m_title->setPadding(SPadding(4.0f, 4.0f, 0.0f, 0.0f));

				m_close = new CButton(m_titleBar);
				m_close->setSize(20.0f, 20.0f);
				m_close->dock(EPosition::Right);
				m_close->setIcon(ESystemIcon::Close);
				m_close->setMargin(SMargin(0.0f, 0.0f, 0.0f, 2.0f));
				m_close->showIcon(true);
				m_close->OnPress = BIND_LISTENER(&CWindow::onCloseButtonPress, this);

				m_innerPanel = new CBase(this);
				m_innerPanel->dock(EPosition::Fill);
				m_innerPanel->enableClip(true);
				m_innerPanel->enableRenderFillRect(true);
			}

			CWindow::~CWindow()
			{

			}

			void CWindow::renderUnder()
			{
				CTheme::getTheme()->drawWindowShadow(getRenderBounds());

				bool isFocussed = isOnTop();
				CTheme::getTheme()->drawWindow(getRenderBounds(), isFocussed);
			}

			void CWindow::touch()
			{
				bringToFront();
				CBase::touch();
			}

			void CWindow::onChildTouched(CBase* child)
			{
				bringToFront();
				CBase::onChildTouched(child);
			}

			void CWindow::onCloseButtonPress(CBase *sender)
			{
				onCloseWindow();
			}

			void CWindow::onCloseWindow()
			{
				remove();
			}
		}
	}
}