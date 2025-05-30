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
#include "Graphics2D/GUI/CGUIElement.h"
#include "Editor/CEditor.h"
#include "CGUIHierachyNode.h"

namespace Skylicht
{
	namespace Editor
	{
		class CGUIHierarchyController
		{
		protected:
			CEditor* m_editor;
			GUI::CTreeControl* m_tree;

			CGUIHierachyNode* m_node;

			GUI::CTreeNode* m_renameNode;

		public:
			CGUIHierarchyController(GUI::CCanvas* canvas, GUI::CTreeControl* tree, CEditor* editor);

			virtual ~CGUIHierarchyController();

			void deleteHierarchyData();

			void setTreeNode(CGUIHierachyNode* node);

			GUI::CTreeNode* addToTreeNode(CGUIHierachyNode* node);

			void rename(GUI::CTreeNode* node);

			static void updateObjectToUI(CGUIElement* object, CGUIHierachyNode* node);

			void updateTreeNode(CGUIElement* object);

			CGUIHierachyNode* getNodeByObject(CGUIElement* object);

		protected:

			void OnHotkey(GUI::CBase* base, const std::string& hotkey);

			void OnKeyPress(GUI::CBase* control, int key, bool press);

			void OnRename(GUI::CBase* control);

			void OnCancelRename(GUI::CBase* control);

			void OnSelectChange(GUI::CBase* control);

			GUI::CTreeNode* buildTreeNode(GUI::CTreeNode* parentGuiNode, CGUIHierachyNode* node);

			void initDragDrop(GUI::CTreeNode* guiNode, CGUIHierachyNode* node);

			void move(CGUIHierachyNode* from, CGUIHierachyNode* target, bool behind);

			void moveToChild(CGUIHierachyNode* from, CGUIHierachyNode* target);
		};
	}
}