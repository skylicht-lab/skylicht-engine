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

#pragma once

#include "GUI/GUI.h"

namespace Skylicht
{
	namespace Editor
	{
		class CHierachyNode
		{
		protected:
			std::wstring m_name;

			std::string m_id;

			void* m_tagData;

			GUI::ESystemIcon m_icon;

			CHierachyNode* m_parent;

			std::vector<CHierachyNode*> m_childs;

			GUI::CTreeNode* m_guiNode;

		public:
			CHierachyNode(CHierachyNode* parent);

			virtual ~CHierachyNode();

			inline void setID(const char* id)
			{
				m_id = id;
			}

			inline const std::string& getID()
			{
				return m_id;
			}

			inline CHierachyNode* getParent()
			{
				return m_parent;
			}

			inline void setTagData(void* data)
			{
				m_tagData = data;
			}

			inline void* getTagData()
			{
				return m_tagData;
			}

			inline void setGUINode(GUI::CTreeNode* node)
			{
				m_guiNode = node;
			}

			inline GUI::CTreeNode* getGUINode()
			{
				return m_guiNode;
			}

			void setName(const wchar_t* name);

			void setIcon(GUI::ESystemIcon icon);

			inline const std::wstring& getName()
			{
				return m_name;
			}

			inline const GUI::ESystemIcon getIcon()
			{
				return m_icon;
			}

			CHierachyNode* addChild();

			bool removeChild(CHierachyNode* child);

			void removeAllChild();

			void bringNextNode(CHierachyNode* position, bool behind);

			void nullGUI();

			inline std::vector<CHierachyNode*>& getChilds()
			{
				return m_childs;
			}
		};
	}
}