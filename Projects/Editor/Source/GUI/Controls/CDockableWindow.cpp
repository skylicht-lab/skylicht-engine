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
#include "CDockableWindow.h"
#include "GUI/Input/CInput.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDockableWindow::CDockableWindow(CDockPanel* parent, float x, float y, float w, float h) :
				CWindow(parent, x, y, w, h),
				m_rootDockPanel(parent)
			{

			}

			CDockableWindow::~CDockableWindow()
			{

			}

			void CDockableWindow::onBeginMoved()
			{
				m_rootDockPanel->showDockHint();
			}

			void CDockableWindow::onMoved()
			{
				m_rootDockPanel->showDockHint();

				SPoint mousePoint = CInput::getInput()->getMousePosition();

				CDockHintIcon* hint = m_rootDockPanel->hitTestDockHint(mousePoint);
				if (hint != NULL)
					m_rootDockPanel->showDockHintWindow(hint, this);
				else
					m_rootDockPanel->hideDockHintWindow();
			}

			void CDockableWindow::onEndMoved()
			{
				SPoint mousePoint = CInput::getInput()->getMousePosition();

				CDockHintIcon* hint = m_rootDockPanel->hitTestDockHint(mousePoint);
				if (hint != NULL)
				{
					EDockHintIcon icon = hint->getIcon();

					switch (icon)
					{
					case Center:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockCenter);
						break;
					case Left:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockLeft);
						break;
					case TargetLeft:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockTargetLeft);
						break;
					case Right:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockRight);
						break;
					case TargetRight:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockTargetRight);
						break;
					case Top:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockTop);
						break;
					case TargetTop:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockTargetTop);
						break;
					case Bottom:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockBottom);
						break;
					case TargetBottom:
						m_rootDockPanel->dockChildWindow(this, CDockPanel::DockTargetBottom);
						break;
					default:
						break;
					}
				}

				m_rootDockPanel->hideDockHint();
			}
		}
	}
}