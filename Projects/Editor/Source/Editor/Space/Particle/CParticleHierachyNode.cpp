/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CParticleHierachyNode.h"

namespace Skylicht
{
	namespace Editor
	{
		CParticleHierachyNode::CParticleHierachyNode(CParticleHierachyNode* parent) :
			m_icon(GUI::ESystemIcon::None),
			m_tagData(NULL),
			m_parentData(NULL),
			m_guiNode(NULL),
			m_parent(parent),
			m_dataType(Unknown),
			m_color(false),
			m_bgColor(255, 45, 140, 95),
			m_iconColor(255, 255, 255, 255)
		{

		}

		CParticleHierachyNode::~CParticleHierachyNode()
		{
			removeAllChild();

			if (m_guiNode != NULL)
				m_guiNode->remove();
		}

		void CParticleHierachyNode::setName(const wchar_t* name)
		{
			m_name = name;

			if (m_guiNode != NULL)
				m_guiNode->setText(name);
		}

		void CParticleHierachyNode::setIcon(Editor::GUI::ESystemIcon icon)
		{
			m_icon = icon;

			if (m_guiNode != NULL)
				m_guiNode->setIcon(icon);
		}

		void CParticleHierachyNode::setIconColor(const GUI::SGUIColor& c)
		{
			m_iconColor = c;

			if (m_guiNode != NULL)
				m_guiNode->setIconColor(c);
		}

		CParticleHierachyNode* CParticleHierachyNode::addChild()
		{
			CParticleHierachyNode* child = new CParticleHierachyNode(this);
			m_childs.push_back(child);
			return child;
		}

		void CParticleHierachyNode::remove()
		{
			getParent()->removeChild(this);
		}

		bool CParticleHierachyNode::removeChild(CParticleHierachyNode* child)
		{
			std::vector<CParticleHierachyNode*>::iterator i = m_childs.begin(), end = m_childs.end();
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

		void CParticleHierachyNode::removeAllChild()
		{
			for (CParticleHierachyNode* i : m_childs)
			{
				delete i;
			}
			m_childs.clear();
		}

		void CParticleHierachyNode::removeAll(EDataType dataType)
		{
			for (int i = (int)m_childs.size() - 1; i >= 0; i--)
			{
				if (m_childs[i]->getTagDataType() == dataType)
				{
					delete m_childs[i];
					m_childs.erase(m_childs.begin() + i);
				}
			}
		}

		bool CParticleHierachyNode::removeChildNoDelete(CParticleHierachyNode* child)
		{
			std::vector<CParticleHierachyNode*>::iterator i = m_childs.begin(), end = m_childs.end();
			while (i != end)
			{
				if ((*i) == child)
				{
					m_childs.erase(i);
					child->removeGUI();
					return true;
				}
				++i;
			}
			return false;
		}

		void CParticleHierachyNode::removeGUI()
		{
			if (m_guiNode != NULL)
			{
				m_guiNode->setSelected(false);
				m_guiNode->remove();
				m_guiNode = NULL;
			}
		}

		void CParticleHierachyNode::bringNextNode(CParticleHierachyNode* position, bool behind)
		{
			std::vector<CParticleHierachyNode*>::iterator i = m_parent->m_childs.begin(), end = m_parent->m_childs.end();
			std::vector<CParticleHierachyNode*>::iterator p;

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

		bool CParticleHierachyNode::isChild(CParticleHierachyNode* child)
		{
			for (CParticleHierachyNode* c : m_childs)
			{
				if (c == child)
					return true;
				else
				{
					bool r = c->isChild(child);
					if (r == true)
						return true;
				}
			}

			return false;
		}

		void CParticleHierachyNode::nullGUI()
		{
			m_guiNode = NULL;
			for (CParticleHierachyNode* i : m_childs)
			{
				i->nullGUI();
			}
		}

		void CParticleHierachyNode::refreshGUI()
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

		CParticleHierachyNode* CParticleHierachyNode::getNodeByTag(void* tag)
		{
			if (getTagData() == tag)
				return this;

			for (CParticleHierachyNode* i : m_childs)
			{
				CParticleHierachyNode* result = i->getNodeByTag(tag);
				if (result != NULL)
					return result;
			}
			return NULL;
		}
	}
}