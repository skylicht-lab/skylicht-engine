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
#include "CTreeNode.h"
#include "CScrollControl.h"
#include "CContentSizeControl.h"
#include "CTreeControl.h"

#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTreeNode::CTreeNode(CBase* parent, CTreeNode* root) :
				CBase(parent),
				m_root(root),
				m_expand(false),
				m_selected(false),
				m_alwayShowExpandButton(false)
			{
				setHeight(20.0f);

				m_parentNode = dynamic_cast<CTreeNode*>(parent);

				m_row = new CTreeRowItem(this, root);
				m_row->OnDown = BIND_LISTENER(&CTreeNode::onDown, this);
				m_row->OnDoubleLeftMouseClick = BIND_LISTENER(&CTreeNode::onDoubleClick, this);

				m_title = new CIconTextItem(this);
				m_title->setMargin(SMargin(16.0f, 0.0f, 0.0f, 0.0f));
				m_title->dock(EPosition::Top);

				m_textEditHelper = new CTextEditHelper(m_row, new CTextBox(m_title), m_title->getTextContainer());

				m_expandButton = new CIconButton(this);
				m_expandButton->setIcon(ESystemIcon::TriangleRight);
				m_expandButton->enableDrawBackground(false);
				m_expandButton->setHidden(true);
				m_expandButton->setPos(0.0f, 0.0f);
				m_expandButton->OnPress = BIND_LISTENER(&CTreeNode::onExpand, this);

				CContentSizeControl* sizeContent = new CContentSizeControl(this);
				sizeContent->setFitType(CContentSizeControl::WrapChildren, CContentSizeControl::WrapChildren);

				m_innerPanel = sizeContent;
				m_innerPanel->dock(EPosition::Top);
				m_innerPanel->setHeight(20.0f);
				m_innerPanel->setMargin(SMargin(CThemeConfig::TreeIndentationSize, 0.0f, 0.0f, 0.0f));
				m_innerPanel->setHidden(true);
				m_innerPanel->setTransparentMouseInput(true);

				setTransparentMouseInput(true);
			}

			CTreeNode::~CTreeNode()
			{
				delete m_textEditHelper;
			}

			void CTreeNode::layout()
			{
				if (m_innerPanel->Children.size() == 0 && m_alwayShowExpandButton == false)
				{
					m_innerPanel->setHidden(true);
					m_expandButton->setHidden(true);
				}
				else
				{
					m_expandButton->setHidden(false);

					if (m_expand == true)
					{
						m_innerPanel->setHidden(false);
						m_expandButton->setIcon(ESystemIcon::TriangleDown);
					}
					else
					{
						m_innerPanel->setHidden(true);
						m_expandButton->setIcon(ESystemIcon::TriangleRight);
					}
				}

				CBase::layout();
			}

			void CTreeNode::postLayout()
			{
				CBase::postLayout();

				if (m_title != NULL)
					m_title->sizeToContents();

				if (m_innerPanel->isHidden() == false)
					m_innerPanel->sizeToChildren(true, true);

				sizeToChildren(true, true);

				m_row->setPos(0.0f, 0.0f);
				m_row->setWidth(width());
				m_row->setHeight(m_title->height());

				SPoint row = m_row->localPosToCanvas();
				SPoint control = m_root->localPosToCanvas();

				CTreeControl* treeControl = (CTreeControl*)(m_root);
				SRect bounds = m_row->getBounds();
				bounds.X = bounds.X - (row.X - control.X);
				bounds.Width = treeControl->getScrollControl()->getInnerWidth();
				m_row->setBounds(bounds);

				m_row->setItemOffset(row.X - control.X + m_title->getMargin().Left);
			}

			CTreeNode* CTreeNode::addNode(const std::wstring& text)
			{
				CTreeNode* node = new CTreeNode(this, m_root);
				node->setText(text);
				node->dock(EPosition::Top);
				return node;
			}

			CTreeNode* CTreeNode::addNode(const std::wstring& text, ESystemIcon icon)
			{
				CTreeNode* node = addNode(text);
				node->setIcon(icon);
				return node;
			}

			CTreeNode* CTreeNode::getChildNodeByLabel(const std::wstring& label)
			{
				for (CBase* c : m_innerPanel->Children)
				{
					CTreeNode* n = dynamic_cast<CTreeNode*>(c);
					if (n != NULL && n->getText() == label)
						return n;
				}

				return NULL;
			}

			void CTreeNode::removeAllTreeNode()
			{
				m_innerPanel->removeAllChildren();
			}

			std::list<CTreeNode*> CTreeNode::getChildNodes()
			{
				std::list<CTreeNode*> result;

				for (CBase* c : m_innerPanel->Children)
				{
					CTreeNode* n = dynamic_cast<CTreeNode*>(c);
					if (n != NULL)
						result.push_back(n);
				}

				return result;
			}

			void CTreeNode::setText(const std::wstring& text)
			{
				if (m_title == NULL)
				{
					// skip at root
					return;
				}

				m_title->setLabel(text);
				invalidate();
			}

			const std::wstring& CTreeNode::getText()
			{
				return m_title->getLabel();
			}

			void CTreeNode::setIcon(ESystemIcon icon)
			{
				if (m_title == NULL)
				{
					// skip at root
					return;
				}

				m_title->setIcon(icon);
				if (icon != ESystemIcon::None)
					m_title->showIcon(true);
				else
					m_title->showIcon(false);

				invalidate();
			}

			void CTreeNode::setIconColor(const SGUIColor& color)
			{
				m_title->setIconColor(color);
			}

			void CTreeNode::onExpand(CBase* base)
			{
				CTreeControl* treeControl = (CTreeControl*)(m_root);
				float y = treeControl->getScrollControl()->getScrollVertical();

				m_expand = !m_expand;

				if (m_expand)
				{
					if (OnExpand != nullptr)
						OnExpand(this);
				}
				else
				{
					if (OnCollapse != nullptr)
						OnCollapse(this);
				}

				forceLayout();

				treeControl->getScrollControl()->layout();
				treeControl->getScrollControl()->setScrollVertical(y);
			}

			void CTreeNode::forceLayout()
			{
				invalidate();

				// OnExpande or OnCollapse maybe create new TreeNode
				// so we need caculate height of TreeControl
				CTreeNode* node = this;

				// each level, we just finish calculate the height of TreeNode [postLayout]
				// so need continue calculate Y (Position::Top) on next call [layout]
				while (node != m_root)
				{
					node->recurseLayout();
					node = node->getParentNode();
				}

				m_root->recurseLayout();
			}

			void CTreeNode::onDoubleClick(CBase* base)
			{
				onExpand(base);
			}

			void CTreeNode::onDown(CBase* base)
			{
				m_root->onNodeClick(base);
				setSelected(!m_selected);
			}

			void CTreeNode::onNodeClick(CBase* base)
			{

			}

			CTreeNode* CTreeNode::getChildSelected()
			{
				if (m_selected)
					return this;

				if (m_innerPanel->Children.size() == 0)
					return NULL;

				for (CBase* c : m_innerPanel->Children)
				{
					CTreeNode* node = dynamic_cast<CTreeNode*>(c);
					if (node && !node->isDisabled())
					{
						CTreeNode* child = node->getChildSelected();
						if (child != NULL)
							return child;
					}
				}

				return NULL;
			}

			CTreeNode* CTreeNode::selectFirstChild()
			{
				if (m_expand == false)
					return NULL;

				if (m_innerPanel->Children.size() == 0)
					return NULL;

				for (CBase* c : m_innerPanel->Children)
				{
					CTreeNode* node = dynamic_cast<CTreeNode*>(c);
					if (node && !node->isDisabled())
					{
						m_root->deselectAll();
						node->setSelected(true);
						return node;
					}
				}

				return NULL;
			}

			CTreeNode* CTreeNode::selectLastChild()
			{
				if (m_expand == false)
					return NULL;

				if (m_innerPanel->Children.size() == 0)
					return NULL;

				List::reverse_iterator i = m_innerPanel->Children.rbegin(), end = m_innerPanel->Children.rend();
				while (i != end)
				{
					CTreeNode* node = dynamic_cast<CTreeNode*>(*i);
					if (node && !node->isDisabled())
					{
						CTreeNode* child = node->selectLastChild();

						if (child == NULL)
						{
							m_root->deselectAll();
							node->setSelected(true);
							return node;
						}
						else
						{
							return child;
						}
					}
					++i;
				}

				return NULL;
			}

			CTreeNode* CTreeNode::selectPrevChild()
			{
				std::list<CTreeNode*> priorityQueue;
				priorityQueue.push_back(this);

				CTreeNode* lastItem = NULL;

				while (priorityQueue.size() > 0)
				{
					CTreeNode* visit = priorityQueue.front();
					priorityQueue.pop_front();

					if (visit->isSelected() == true)
					{
						if (lastItem == NULL)
							return NULL;

						m_root->deselectAll();
						lastItem->setSelected(true);
						return lastItem;
					}

					if (visit->m_expand && visit->m_innerPanel->Children.size() > 0)
					{
						std::list<CTreeNode*>::iterator i = priorityQueue.begin();

						for (CBase* c : visit->m_innerPanel->Children)
						{
							CTreeNode* node = dynamic_cast<CTreeNode*>(c);
							if (node && !node->isDisabled())
							{
								i = priorityQueue.insert(i, node);
								++i;
							}
						}
					}

					if (visit != this)
						lastItem = visit;
				}

				return NULL;
			}

			CTreeNode* CTreeNode::selectNextChild()
			{
				std::list<CTreeNode*> priorityQueue;
				priorityQueue.push_back(this);

				bool selectNext = false;

				while (priorityQueue.size() > 0)
				{
					CTreeNode* visit = priorityQueue.front();
					priorityQueue.pop_front();

					if (selectNext == true)
					{
						m_root->deselectAll();
						visit->setSelected(true);
						return visit;
					}

					if (visit->isSelected() == true)
					{
						selectNext = true;
					}

					if (visit->m_expand && visit->m_innerPanel->Children.size() > 0)
					{
						std::list<CTreeNode*>::iterator i = priorityQueue.begin();

						for (CBase* c : visit->m_innerPanel->Children)
						{
							CTreeNode* node = dynamic_cast<CTreeNode*>(c);
							if (node && !node->isDisabled())
							{
								i = priorityQueue.insert(i, node);
								++i;
							}
						}
					}
				}

				return NULL;
			}

			void CTreeNode::setSelected(bool b, bool callEvent)
			{
				if (m_selected == b)
					return;

				m_selected = b;

				if (m_row != NULL)
					m_row->setToggle(b);

				if (callEvent == true)
				{
					if (b == true)
					{
						if (OnSelected != nullptr)
							OnSelected(this);
					}
					else
					{
						if (OnUnselected != nullptr)
							OnUnselected(this);
					}

					if (OnSelectChange != nullptr)
						OnSelectChange(this);
				}

				invalidate();
			}

			void CTreeNode::deselectAll(bool callEvent)
			{
				setSelected(false, callEvent);

				for (CBase* c : m_innerPanel->Children)
				{
					CTreeNode* node = dynamic_cast<CTreeNode*>(c);
					if (node)
					{
						node->deselectAll(callEvent);
					}
				}
			}

			bool CTreeNode::hoverOnChild()
			{
				return m_expandButton->isHovered() || m_row->isHovered();
			}
		}
	}
}