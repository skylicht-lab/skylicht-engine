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
#include "CTreeNode.h"

#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTreeNode::CTreeNode(CBase *parent, CTreeNode *root) :
				CBase(parent),
				m_root(root),
				m_expand(false)
			{
				m_title = new CButton(this);
				m_title->dock(EPosition::Top);
				m_title->enableDrawBackground(false);
				m_title->setIconMargin(SMargin(0.0f, 0.0f, 5.0f, 0.0f));
				m_title->setLabelMargin(SMargin(0.0f, 4.0f, 0.0f, 0.0f));
				m_title->setMargin(SMargin(16.0f, 0.0f, 0.0f, 0.0f));

				m_expandButton = new CIconButton(this);
				m_expandButton->setIcon(ESystemIcon::TriangleRight);
				m_expandButton->enableDrawBackground(false);
				m_expandButton->setHidden(true);
				m_expandButton->setPos(0.0f, 0.0f);
				m_expandButton->OnPress = BIND_LISTENER(&CTreeNode::onExpand, this);

				m_innerPanel = new CBase(this);
				m_innerPanel->dock(EPosition::Top);
				m_innerPanel->setHeight(20.0f);
				m_innerPanel->setMargin(SMargin(CThemeConfig::TreeIndentationSize, 0.0f, 0.0f, 0.0f));
				m_innerPanel->setHidden(true);
			}

			CTreeNode::~CTreeNode()
			{

			}

			void CTreeNode::layout()
			{
				if (m_innerPanel->Children.size() == 0)
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
				if (m_innerPanel->isHidden() == false)
					m_innerPanel->sizeToChildren(false, true);

				sizeToChildren(false, true);
				CBase::postLayout();
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
				CTreeNode *node = addNode(text);
				node->setIcon(icon);
				return node;
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

			void CTreeNode::onExpand(CBase *base)
			{
				m_expand = !m_expand;
				invalidate();
			}
		}
	}
}