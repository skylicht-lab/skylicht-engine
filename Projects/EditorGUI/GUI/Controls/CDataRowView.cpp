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
#include "CDataRowView.h"
#include "CDataGridView.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDataRowView::CDataRowView(CBase* base, CDataGridView* gridView, const wchar_t* label, ESystemIcon icon) :
				CButton(base),
				m_owner(gridView),
				m_rowID(-1)
			{
				setSize(80.0f, 20.0f);

				setIsToggle(true);
				enableDrawBackground(false);

				m_label->setString(label);
				m_label->dock(EPosition::Left);

				m_icon->setIcon(icon);

				if (icon != ESystemIcon::None)
					showIcon(true);

				setHoverColor(CThemeConfig::ListItemBackgroundHoverColor);
				setPressColor(CThemeConfig::ListItemBackgroundFocusColor);

				m_childs.push_back(m_label);
				for (int i = 1; i < gridView->getNumColumn(); i++)
				{
					m_childs.push_back(NULL);
				}
			}

			CDataRowView::~CDataRowView()
			{

			}

			void CDataRowView::layout()
			{
				CBase::layout();

				sizeToContents();

				updateColumnSize();
			}

			void CDataRowView::onBoundsChanged(const SRect& oldBounds)
			{
				CBase::onBoundsChanged(oldBounds);
			}

			void CDataRowView::renderUnder()
			{
				SRect r = getRenderBounds();

				if (m_rowID % 2 == 0)
					CRenderer::getRenderer()->drawFillRect(r, SGUIColor(255, 40, 40, 40));
				else
					CRenderer::getRenderer()->drawFillRect(r, SGUIColor(255, 45, 45, 45));

				if (isHovered() || m_toggleStatus)
					renderBackground();
			}

			void CDataRowView::renderBackground()
			{
				SGUIColor c = m_color;

				if (m_pressed == true)
					c = m_pressColor;
				else
					c = m_hoverColor;

				if (isToggle())
				{
					if (m_toggleStatus)
					{
						if (m_owner->isKeyboardFocus() == true)
							c = m_pressColor;
						else
							c = CThemeConfig::ListItemBackgroundUnfocusColor;
					}
				}

				SRect r = getRenderBounds();
				r.X = r.X + 1.0f;
				r.Y = r.Y + 1.0f;
				r.Width = r.Width - 2.0f;
				r.Height = r.Height - 1.0f;

				CTheme::getTheme()->drawButton(r, c);
			}

			void CDataRowView::onMouseClickRight(float x, float y, bool down)
			{
				if (isDisabled())
					return;

				CButton::onMouseClickRight(x, y, down);

				if (down == false)
				{
					if (m_owner->OnItemContextMenu != nullptr)
						m_owner->OnItemContextMenu(this);
				}
			}

			CDataGridView* CDataRowView::getDataGridView()
			{
				return m_owner;
			}

			CBase* CDataRowView::getControl(int col)
			{
				return m_childs[col];
			}

			CBase* CDataRowView::setControl(int col, CBase* control)
			{
				m_childs[col] = control;
				return control;
			}

			CLabel* CDataRowView::setLabel(int col, const wchar_t* text)
			{
				CLabel* label = dynamic_cast<CLabel*>(m_childs[col]);
				if (label == NULL)
				{
					label = new CLabel(this);
					label->setPadding(SMargin(8.0f, 2.0f, 0.0f, 0.0f));

					if (m_childs[col] != NULL)
						m_childs[col]->remove();

					m_childs[col] = label;
				}
				label->setString(text);
				return label;
			}

			void CDataRowView::updateColumnSize()
			{
				float labelWidth = m_label->right();
				float colWidth = m_owner->getColumnWidth(0);

				if (labelWidth > colWidth)
				{
					float delta = labelWidth - colWidth;
					m_label->setWidth(m_label->width() - delta);
				}
				else
				{
					float delta = colWidth - labelWidth;
					m_label->setWidth(m_label->width() + delta);
				}

				float x = colWidth;
				for (int i = 1, n = (int)m_childs.size(); i < n; i++)
				{
					colWidth = m_owner->getColumnWidth(i);
					if (m_childs[i] != NULL)
					{
						CBase* base = m_childs[i];
						base->setPos(x, 0.0f);
						base->setWidth(colWidth);
						base->setHeight(height());
					}
					x = x + colWidth;
				}
			}
		}
	}
}