/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CDialogWindow.h"
#include "GUI/CGUIContext.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDialogWindow::CDialogWindow(CBase* parent, float x, float y, float w, float h) :
				CWindow(parent, x, y, w, h),
				m_blink(0),
				m_blinkTime(0.0f)
			{
				setResizable(false);
				parent->getCanvas()->setTopmostDialog(this);
			}

			CDialogWindow::~CDialogWindow()
			{
				if (m_parent->getCanvas() != NULL &&
					m_parent->getCanvas()->getTopmostDialog() == this)
				{
					m_parent->getCanvas()->setTopmostDialog(NULL);
				}
			}

			void CDialogWindow::setHidden(bool hidden)
			{
				CWindow::setHidden(hidden);

				if (hidden == true)
				{
					if (m_parent->getCanvas()->getTopmostDialog() == this)
						m_parent->getCanvas()->setTopmostDialog(NULL);
				}
				else
				{
					m_parent->getCanvas()->setTopmostDialog(this);
				}
			}

			void CDialogWindow::think()
			{
				CWindow::think();

				float dt = CGUIContext::getDeltaTime();
				if (m_blink > 0)
				{
					m_blinkTime = m_blinkTime + dt;
					if (m_blinkTime > 80.0f)
					{
						m_blink--;
						m_blinkTime = 0.0f;
					}

					if (m_title != NULL)
					{
						if (m_blink % 2 == 0)
						{
							m_title->setColor(CThemeConfig::DefaultTextColor);
						}
						else
						{
							m_title->setColor(CThemeConfig::DisableTextColor);
						}
					}
				}
			}

			void CDialogWindow::blinkCaption()
			{
				m_blink = 6;
				m_blinkTime = 0.0f;
			}
		}
	}
}