/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CTreeRowItem.h"
#include "CTreeNode.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTreeRowItem::CTreeRowItem(CBase* base, CBase* root) :
				CButton(base),
				m_root(root),
				m_itemOffsetX(0.0f),
				m_lineColor(255, 255, 255, 255),
				m_drawLineTop(false),
				m_drawLineBottom(false)
			{
				setIsToggle(true);
				enableDrawBackground(false);

				setHoverColor(CThemeConfig::ListItemBackgroundHoverColor);
				setPressColor(CThemeConfig::ListItemBackgroundFocusColor);
			}

			CTreeRowItem::~CTreeRowItem()
			{

			}

			void CTreeRowItem::renderUnder()
			{
				CTreeNode* treeNode = dynamic_cast<CTreeNode*>(m_parent);
				if (treeNode != NULL)
				{
					if (treeNode->hoverOnChild() || m_toggleStatus)
					{
						renderBackground();
					}
				}

				if (m_drawBackground)
				{
					CButton::renderUnder();
				}
			}

			void CTreeRowItem::renderBackground()
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
						if (m_root->isKeyboardFocus() == true)
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

				m_renderOffset = CRenderer::getRenderer()->getRenderOffset();
			}

			void CTreeRowItem::renderOver()
			{
				CBase::renderOver();

				SPoint currentOffset = CRenderer::getRenderer()->getRenderOffset();
				CRenderer::getRenderer()->setRenderOffset(m_renderOffset);

				SRect r = getRenderBounds();
				r.X = r.X + 1.0f;
				r.Y = r.Y + 1.0f;
				r.Width = r.Width - 2.0f;
				r.Height = r.Height - 1.0f;

				float y = r.Y;
				float height = r.Height;

				r.X = r.X + m_itemOffsetX;
				r.Width = r.Width - m_itemOffsetX;
				r.Height = 2.0f;

				// top
				if (m_drawLineTop)
				{
					r.Y = r.Y - 1.0f;
					CRenderer::getRenderer()->drawFillRect(r, m_lineColor);
				}

				// bottom
				if (m_drawLineBottom)
				{
					r.Y = y + height;
					CRenderer::getRenderer()->drawFillRect(r, m_lineColor);
				}

				CRenderer::getRenderer()->setRenderOffset(currentOffset);
			}

			void CTreeRowItem::onMouseClickRight(float x, float y, bool down)
			{
				if (isDisabled())
					return;

				CButton::onMouseClickRight(x, y, down);

				if (down == false)
				{
					CTreeNode* treeRoot = dynamic_cast<CTreeNode*>(m_root);
					if (treeRoot->OnItemContextMenu != nullptr)
						treeRoot->OnItemContextMenu(this);
				}
			}

			CTreeNode* CTreeRowItem::getNode()
			{
				return dynamic_cast<CTreeNode*>(m_parent);
			}
		}
	}
}