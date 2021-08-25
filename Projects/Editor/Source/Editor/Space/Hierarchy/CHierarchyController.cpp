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
#include "CHierarchyController.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CHierarchyController::CHierarchyController(GUI::CCanvas* canvas, GUI::CTreeControl* tree) :
			m_canvas(canvas),
			m_tree(tree),
			m_node(NULL)
		{
			m_tree->OnKeyPress = std::bind(
				&CHierarchyController::OnKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);

			m_tree->OnAcceptDragDrop = [](const std::string& name) {
				if (name == "HierarchyNode")
					return true;
				return false;
			};
		}

		CHierarchyController::~CHierarchyController()
		{
			if (m_node != NULL)
				m_node->nullGUI();
		}

		void CHierarchyController::setHierarchyNode(CHierachyNode* node)
		{
			if (m_node != node)
			{
				if (m_node != NULL)
					m_node->nullGUI();
			}

			m_node = node;

			// remove all node
			m_tree->removeAllTreeNode();

			// add child nodes
			GUI::CTreeNode* root0 = buildHierarchyNode(m_tree, m_node);

			// expand tree
			root0->expand(false);
			for (GUI::CTreeNode* root1 : root0->getChildNodes())
				root1->expand(false);
		}

		GUI::CTreeNode* CHierarchyController::buildHierarchyNode(GUI::CTreeNode* parentGuiNode, CHierachyNode* node)
		{
			// add node
			GUI::CTreeNode* guiNode = parentGuiNode->addNode(node->getName(), node->getIcon());

			// apply select event
			guiNode->OnSelectChange = BIND_LISTENER(&CHierarchyController::OnSelectChange, this);

			// link data gui to node
			guiNode->tagData(node);

			// set drag drop data
			initDragDrop(guiNode, node);

			// link data node to gui
			node->setGUINode(guiNode);

			// apply active color
			if (node->HaveColor())
			{
				guiNode->getRowItem()->setColor(node->getBGColor());
				guiNode->getRowItem()->enableDrawBackground(true);
			}
			else
			{
				guiNode->getRowItem()->enableDrawBackground(false);
			}

			// loop all childs
			std::vector<CHierachyNode*>& childs = node->getChilds();
			for (CHierachyNode* child : childs)
			{
				buildHierarchyNode(guiNode, child);
			}

			// expand
			parentGuiNode->expand(false);

			return guiNode;
		}

		GUI::CTreeNode* CHierarchyController::add(CHierachyNode* node)
		{
			CHierachyNode* parent = node->getParent();
			GUI::CTreeNode* parentGuiNode = parent->getGUINode();

			GUI::CTreeNode* ret = buildHierarchyNode(parentGuiNode, node);
			m_tree->deselectAll();
			m_tree->getScrollControl()->scrollToItem(ret);

			ret->setSelected(true);
			return ret;
		}

		void CHierarchyController::OnKeyPress(GUI::CBase* control, int key, bool press)
		{
			GUI::CTreeControl* tree = dynamic_cast<GUI::CTreeControl*>(control);
			if (tree == NULL)
				return;

			if (key == GUI::KEY_F2)
			{
				rename(tree->getChildSelected());
			}
		}

		void CHierarchyController::rename(GUI::CTreeNode* node)
		{
			if (node != NULL)
			{
				m_renameNode = node;

				node->getTextEditHelper()->beginEdit(
					BIND_LISTENER(&CHierarchyController::OnRename, this),
					BIND_LISTENER(&CHierarchyController::OnCancelRename, this)
				);
			}
		}

		void CHierarchyController::OnRename(GUI::CBase* control)
		{
			GUI::CTextBox* textbox = dynamic_cast<GUI::CTextBox*>(control);

			std::wstring newNameW = textbox->getString();

			CHierachyNode* node = (CHierachyNode*)m_renameNode->getTagData();
			node->setName(newNameW.c_str());

			if (node->OnUpdate != nullptr)
				node->OnUpdate(node);

			m_tree->focus();
		}

		void CHierarchyController::OnCancelRename(GUI::CBase* control)
		{
			m_tree->focus();
		}

		void CHierarchyController::OnSelectChange(GUI::CBase* control)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(control);
			if (treeNode != NULL)
			{
				CHierachyNode* node = (CHierachyNode*)treeNode->getTagData();
				if (node->OnSelected != nullptr)
					node->OnSelected(node, treeNode->isSelected());
			}
		}

		void CHierarchyController::initDragDrop(GUI::CTreeNode* guiNode, CHierachyNode* node)
		{
			GUI::CTreeRowItem* rowItem = guiNode->getRowItem();

			GUI::SDragDropPackage* dragDrop = rowItem->setDragDropPackage("HierarchyNode", node);

			dragDrop->DrawControl = guiNode->getTextItem();

			rowItem->OnAcceptDragDrop = [node](const std::string& name)
			{
				if (node->getTagDataType() == CHierachyNode::GameObject)
				{
					if (name == "HierarchyNode")
						return true;
				}
				return false;
			};

			rowItem->OnDragDropHover = [rowItem](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				// rowItem->enableDrawLine(true, false);
			};

			rowItem->OnDragDropOut = [rowItem](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				rowItem->enableDrawLine(false, false);
			};

			rowItem->OnDrop = [rowItem](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				rowItem->enableDrawLine(false, false);
			};
		}
	}
}