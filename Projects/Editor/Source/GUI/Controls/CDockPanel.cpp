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
#include "CDockPanel.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDockPanel::CDockPanel(CBase *parent) :
				CBase(parent)
			{
				bool root = parent == NULL;

				m_dockHint[0] = new CDockHintIcon(this, EDockHintIcon::Center);
				if (root == true)
				{
					m_dockHint[1] = new CDockHintIcon(this, EDockHintIcon::Left);
					m_dockHint[2] = new CDockHintIcon(this, EDockHintIcon::Right);
					m_dockHint[3] = new CDockHintIcon(this, EDockHintIcon::Top);
					m_dockHint[4] = new CDockHintIcon(this, EDockHintIcon::Bottom);
				}
				else
				{
					m_dockHint[1] = new CDockHintIcon(this, EDockHintIcon::TargetLeft);
					m_dockHint[2] = new CDockHintIcon(this, EDockHintIcon::TargetRight);
					m_dockHint[3] = new CDockHintIcon(this, EDockHintIcon::TargetTop);
					m_dockHint[4] = new CDockHintIcon(this, EDockHintIcon::TargetBottom);
				}

				hideDockHint();
			}

			CDockPanel::~CDockPanel()
			{

			}

			void CDockPanel::layout()
			{
				CBase::layout();

				bool root = m_parent == NULL;

				SPoint center(
					m_bounds.X + m_bounds.Width * 0.5f,
					m_bounds.Y + m_bounds.Height * 0.5f
				);

				float w = m_dockHint[0]->width();
				float h = m_dockHint[0]->height();

				if (root == true)
				{
					float padding = 20.0f;
					m_dockHint[0]->setPos(center.X - w * 0.5f, center.Y - h * 0.5f);
					m_dockHint[1]->setPos(padding, center.Y - h * 0.5f);
					m_dockHint[2]->setPos(width() - padding - w, center.Y - h * 0.5f);
					m_dockHint[3]->setPos(center.X - w * 0.5f, padding);
					m_dockHint[4]->setPos(center.X - w * 0.5f, height() - padding - h);
				}
				else
				{
					float padding = 50.0f;
					m_dockHint[0]->setPos(center.X - w * 0.5f, center.Y - h * 0.5f);
					m_dockHint[1]->setPos(center.X - w * 0.5f - padding, center.Y - h * 0.5f);
					m_dockHint[2]->setPos(center.X - w * 0.5f + padding, center.Y - h * 0.5f);
					m_dockHint[3]->setPos(center.X - w * 0.5f, center.Y - h * 0.5f - padding);
					m_dockHint[4]->setPos(center.X - w * 0.5f, center.Y - h * 0.5f + padding);
				}
			}

			void CDockPanel::showDockHint()
			{
				for (int i = 0; i < 5; i++)
				{
					m_dockHint[i]->setHidden(false);
					m_dockHint[i]->bringToFront();
				}
			}

			void CDockPanel::hideDockHint()
			{
				for (int i = 0; i < 5; i++)
				{
					m_dockHint[i]->setHidden(true);
				}
			}
		}
	}
}