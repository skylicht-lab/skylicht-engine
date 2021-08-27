/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CHierachyNode.h"

namespace Skylicht
{
	namespace Editor
	{
		CHierachyNode::CHierachyNode(CHierachyNode* parent) :
			m_icon(GUI::ESystemIcon::None),
			m_tagData(NULL),
			m_guiNode(NULL),
			m_parent(parent),
			m_dataType(Unknown),
			m_color(false),
			m_bgColor(255, 45, 140, 95)
		{

		}

		CHierachyNode::~CHierachyNode()
		{
			removeAllChild();

			if (m_guiNode != NULL)
				m_guiNode->remove();
		}

		void CHierachyNode::setName(const wchar_t* name)
		{
			m_name = name;

			if (m_guiNode != NULL)
				m_guiNode->setText(name);
		}

		void CHierachyNode::setIcon(Editor::GUI::ESystemIcon icon)
		{
			m_icon = icon;

			if (m_guiNode != NULL)
				m_guiNode->setIcon(icon);
		}

		CHierachyNode* CHierachyNode::addChild()
		{
			CHierachyNode* child = new CHierachyNode(this);
			m_childs.push_back(child);
			return child;
		}

		void CHierachyNode::remove()
		{
			getParent()->removeChild(this);
		}

		bool CHierachyNode::removeChild(CHierachyNode* child)
		{
			std::vector<CHierachyNode*>::iterator i = m_childs.begin(), end = m_childs.end();
			while (i != end)
			{
				if ((*i) == child)
				{
					m_childs.erase(i);
					delete child;
					return true;
				}
				++i;
			}
			return false;
		}

		void CHierachyNode::removeAllChild()
		{
			for (CHierachyNode* i : m_childs)
			{
				delete i;
			}
			m_childs.clear();
		}

		bool CHierachyNode::removeChildNoDelete(CHierachyNode* child)
		{
			std::vector<CHierachyNode*>::iterator i = m_childs.begin(), end = m_childs.end();
			while (i != end)
			{
				if ((*i) == child)
				{
					m_childs.erase(i);

					if (m_guiNode != NULL)
					{
						m_guiNode->remove();
						m_guiNode = NULL;
					}

					return true;
				}
				++i;
			}
			return false;
		}

		void CHierachyNode::removeGUI()
		{
			if (m_guiNode != NULL)
			{
				m_guiNode->remove();
				m_guiNode = NULL;
			}
		}

		void CHierachyNode::bringNextNode(CHierachyNode* position, bool behind)
		{
			std::vector<CHierachyNode*>::iterator i = m_parent->m_childs.begin(), end = m_parent->m_childs.end();
			std::vector<CHierachyNode*>::iterator p;

			while (i != end)
			{
				if ((*i) == this)
				{
					m_parent->m_childs.erase(i);
					break;
				}
				++i;
			}

			m_parent = position->m_parent;

			i = m_parent->m_childs.begin();
			end = m_parent->m_childs.end();
			while (i != end)
			{
				if ((*i) == position)
				{
					p = i;
					break;
				}
				++i;
			}

			if (p != end)
			{
				if (behind)
				{
					m_parent->m_childs.insert(++p, this);
				}
				else
				{
					m_parent->m_childs.insert(p, this);
				}
			}
		}

		void CHierachyNode::nullGUI()
		{
			m_guiNode = NULL;
			for (CHierachyNode* i : m_childs)
			{
				i->nullGUI();
			}
		}

		void CHierachyNode::refreshGUI()
		{
			if (m_guiNode != NULL)
			{
				// update text & icon
				m_guiNode->setText(m_name);
				m_guiNode->setIcon(m_icon);

				// update background color
				if (m_color)
				{
					m_guiNode->getRowItem()->setColor(m_bgColor);
					m_guiNode->getRowItem()->enableDrawBackground(true);
				}
				else
				{
					m_guiNode->getRowItem()->enableDrawBackground(false);
				}
			}
		}

		CHierachyNode* CHierachyNode::getNodeByTag(void* tag)
		{
			if (getTagData() == tag)
				return this;

			for (CHierachyNode* i : m_childs)
			{
				CHierachyNode* result = i->getNodeByTag(tag);
				if (result != NULL)
					return result;
			}
			return NULL;
		}
	}
}