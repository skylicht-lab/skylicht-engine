/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
		class CParticleHierachyNode
		{
		public:
			enum EDataType
			{
				Unknown,
				Particle,
				Group,
				Zone,
				Emitters,
				Emitter,
				Renderer,
				Models,
				Model,
				Gravity,
				Orientation
			};

			std::function<void(CParticleHierachyNode*)> OnUpdate;
			std::function<void(CParticleHierachyNode*, bool)> OnSelected;

		protected:
			std::wstring m_name;

			std::string m_id;

			void* m_tagData;

			void* m_parentData;

			EDataType m_dataType;

			bool m_color;

			GUI::SGUIColor m_bgColor;

			GUI::SGUIColor m_iconColor;

			GUI::ESystemIcon m_icon;

			CParticleHierachyNode* m_parent;

			std::vector<CParticleHierachyNode*> m_childs;

			GUI::CTreeNode* m_guiNode;

		public:
			CParticleHierachyNode(CParticleHierachyNode* parent);

			virtual ~CParticleHierachyNode();

			inline CParticleHierachyNode* getParent()
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

			inline void setParentData(void* data)
			{
				m_parentData = data;
			}

			inline void* getParentData()
			{
				return m_parentData;
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

			void setIconColor(const GUI::SGUIColor& c);

			inline const std::wstring& getName()
			{
				return m_name;
			}

			inline const GUI::ESystemIcon getIcon()
			{
				return m_icon;
			}

			inline bool haveColor()
			{
				return m_color;
			}

			inline void enableColor(bool b)
			{
				m_color = b;
			}

			inline GUI::SGUIColor& getBGColor()
			{
				return m_bgColor;
			}

			inline void setBGColor(const GUI::SGUIColor& c)
			{
				m_bgColor = c;
			}

			CParticleHierachyNode* addChild();

			void remove();

			bool removeChild(CParticleHierachyNode* child);

			void removeAllChild();

			void removeAll(EDataType dataType);

			bool removeChildNoDelete(CParticleHierachyNode* child);

			void removeGUI();

			void nullGUI();

			void refreshGUI();

			bool isChild(CParticleHierachyNode* child);

			inline std::vector<CParticleHierachyNode*>& getChilds()
			{
				return m_childs;
			}

			CParticleHierachyNode* getNodeByTag(void* tag);

			void bringNextNode(CParticleHierachyNode* position, bool behind);
		};
	}
}