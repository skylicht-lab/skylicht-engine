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

#pragma once

#include "GUI/GUI.h"

namespace Skylicht
{
	namespace Editor
	{
		class CHierachyNode
		{
			friend class CHierarchyController;

		public:
			enum EDataType
			{
				Unknown,
				Scene,
				Zone,
				Container,
				GameObject,
				Entity,
			};

			std::function<void(CHierachyNode*)> OnUpdate;
			std::function<void(CHierachyNode*, bool)> OnSelected;
			std::function<void(CHierachyNode*)> OnDoubleClick;

		protected:
			std::wstring m_name;

			std::string m_id;

			void* m_tagData;

			EDataType m_dataType;

			bool m_color;

			GUI::SGUIColor m_bgColor;
			GUI::SGUIColor m_textColor;
			GUI::SGUIColor m_iconColor;

			GUI::ESystemIcon m_icon;

			bool m_haveSubIcon;
			GUI::ESystemIcon m_subIcon;

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

			inline void setTagData(void* data, EDataType type)
			{
				m_tagData = data;
				m_dataType = type;
			}

			inline void* getTagData()
			{
				return m_tagData;
			}

			inline bool isTagGameObject()
			{
				return m_dataType == GameObject || m_dataType == Container || m_dataType == Zone;
			}

			inline bool isTagScene()
			{
				return m_dataType == Scene;
			}

			inline bool isTagEntity()
			{
				return m_dataType == Entity;
			}

			inline EDataType getTagDataType()
			{
				return m_dataType;
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

			void setSubIcon(GUI::ESystemIcon icon);

			void setIconColor(const GUI::SGUIColor& c);

			void setTextColor(const GUI::SGUIColor& c);

			inline const std::wstring& getName()
			{
				return m_name;
			}

			inline const GUI::ESystemIcon getIcon()
			{
				return m_icon;
			}

			inline const GUI::ESystemIcon getSubIcon()
			{
				return m_subIcon;
			}

			inline bool haveColor()
			{
				return m_color;
			}

			inline bool haveSubIcon()
			{
				return m_haveSubIcon;
			}

			inline void showSubIcon(bool b)
			{
				m_haveSubIcon = b;
			}

			inline void enableColor(bool b)
			{
				m_color = b;
			}

			inline GUI::SGUIColor& getIconColor()
			{
				return m_iconColor;
			}

			inline GUI::SGUIColor& getTextColor()
			{
				return m_textColor;
			}

			inline GUI::SGUIColor& getBGColor()
			{
				return m_bgColor;
			}

			inline void setBGColor(const GUI::SGUIColor& c)
			{
				m_bgColor = c;
			}

			CHierachyNode* addChild();

			void remove();

			bool removeChild(CHierachyNode* child);

			void removeAllChild();

			void removeAll(EDataType dataType);

			bool removeChildNoDelete(CHierachyNode* child);

			void removeGUI();

			void nullGUI();

			void refreshGUI();

			bool isChild(CHierachyNode* child);

			inline std::vector<CHierachyNode*>& getChilds()
			{
				return m_childs;
			}

			CHierachyNode* getNodeByTag(void* tag);

			void bringToNext(CHierachyNode* node, CHierachyNode* position, bool behind);

			void bringToChild(CHierachyNode* node);
		};
	}
}