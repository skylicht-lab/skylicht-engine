/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

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
#include "CHierachyNode.h"

namespace Skylicht
{
	namespace Editor
	{
		CHierachyNode::CHierachyNode(CHierachyNode* parent) :
			m_icon(GUI::ESystemIcon::None),
			m_tagData(NULL),
			m_guiNode(NULL),
			m_parent(parent)
		{

		}

		CHierachyNode::~CHierachyNode()
		{
			removeAllChild();

			if (m_guiNode)
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

		void CHierachyNode::bringNextNode(CHierachyNode* position, bool behind)
		{
			std::vector<CHierachyNode*>::iterator i = m_parent->m_childs.begin(), end = m_parent->m_childs.end();
			std::vector<CHierachyNode*>::iterator p = end;
			std::vector<CHierachyNode*>::iterator m = end;

			while (i != end)
			{
				if ((*i) == this)
					m = i;

				if ((*i) == position)
					p = i;

				if (p != end && m != end)
					break;
			}

			if (p != end && m != end)
			{
				m_parent->m_childs.erase(m);

				if (!behind)
				{
					if (p == m_parent->m_childs.begin())
					{
						m_parent->m_childs.insert(m_parent->m_childs.begin(), this);
					}
					else
					{
						m_parent->m_childs.insert(++p, this);
					}
				}
				else
				{
					m_parent->m_childs.insert(p, this);
				}

				if (m_guiNode)
					m_guiNode->bringNextToControl(position->getGUINode(), behind);
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
	}
}