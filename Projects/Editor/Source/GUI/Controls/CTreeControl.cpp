/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CTreeControl.h"
#include "CScrollControl.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTreeControl::CTreeControl(CBase *parent) :
				CTreeNode(parent, this)
			{
				m_title->remove();
				m_expandButton->remove();
				m_innerPanel->remove();
				m_row->remove();

				m_title = NULL;
				m_expandButton = NULL;
				m_innerPanel = NULL;
				m_row = NULL;
				m_expand = true;

				m_scrollControl = new CScrollControl(this);
				m_scrollControl->dock(EPosition::Fill);
				m_scrollControl->showScrollBar(true, true);
				m_scrollControl->getVerticalSroll()->setNudgeAmount(40.0f);

				m_innerPanel = m_scrollControl->getInnerPanel();

				setTransparentMouseInput(false);
				setKeyboardInputEnabled(true);
			}

			CTreeControl::~CTreeControl()
			{

			}

			void CTreeControl::layout()
			{
				CBase::layout();
			}

			void CTreeControl::postLayout()
			{
				CBase::postLayout();

				sizeToChildren(true, true);
			}

			void CTreeControl::onNodeClick(CBase *base)
			{
				deselectAll();
			}

			void CTreeControl::deselectAll()
			{
				for (CBase *c : m_innerPanel->Children)
				{
					CTreeNode *node = dynamic_cast<CTreeNode*>(c);
					if (node)
					{
						node->deselectAll();
					}
				}
			}

			bool CTreeControl::onKeyUp(bool down)
			{
				if (down)
				{
					CTreeNode *child = selectPrevChild();
					if (child != NULL)
					{
						m_scrollControl->scrollToItem(child->getTextItem());
					}
				}

				return true;
			}

			bool CTreeControl::onKeyDown(bool down)
			{
				if (down)
				{
					CTreeNode *child = selectNextChild();
					if (child != NULL)
					{
						m_scrollControl->scrollToItem(child->getTextItem());
					}
				}

				return true;
			}

			bool CTreeControl::onKeyHome(bool down)
			{
				if (down)
				{
					for (CBase *c : m_innerPanel->Children)
					{
						CTreeNode *node = dynamic_cast<CTreeNode*>(c);
						if (node && !node->isDisabled())
						{
							deselectAll();
							node->setSelected(true);
							m_scrollControl->scrollToItem(node->getTextItem());
							return true;
						}
					}
				}

				return true;
			}

			bool CTreeControl::onKeyEnd(bool down)
			{
				if (down)
				{
					List::reverse_iterator i = m_innerPanel->Children.rbegin(), end = m_innerPanel->Children.rend();

					while (i != end)
					{
						CTreeNode *node = dynamic_cast<CTreeNode*>(*i);
						if (node && !node->isDisabled())
						{
							CTreeNode *child = node->selectLastChild();
							if (child != NULL)
							{
								m_scrollControl->scrollToItem(child->getTextItem());
							}
							else
							{
								deselectAll();
								node->setSelected(true);
								m_scrollControl->scrollToItem(node->getTextItem());
							}
							return true;
						}
						++i;
					}
				}
				return true;
			}

			bool CTreeControl::onKeyLeft(bool down)
			{
				if (down)
				{
					CTreeNode* node = getChildSelected();
					if (node != NULL)
					{
						if (node->haveChild() && node->isExpand())
							node->collapse();
						else
						{
							// select parent
							CTreeNode *parent = node->getParentNode();
							if (parent != NULL)
							{
								deselectAll();
								parent->setSelected(true);
								m_scrollControl->scrollToItem(parent->getTextItem());
							}
						}
					}
				}
				return true;
			}

			bool CTreeControl::onKeyRight(bool down)
			{
				if (down)
				{
					CTreeNode* node = getChildSelected();
					if (node != NULL)
					{
						node->expand();
					}
				}
				return true;
			}
		}
	}
}