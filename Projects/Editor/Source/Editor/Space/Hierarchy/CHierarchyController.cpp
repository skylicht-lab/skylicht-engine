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

#include "GameObject/CZone.h"

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

			bool allowDrag = true;

			if (node == m_node)
				allowDrag = false;

			if (allowDrag)
			{
				GUI::SDragDropPackage* dragDrop = rowItem->setDragDropPackage("HierarchyNode", node);
				dragDrop->DrawControl = guiNode->getTextItem();
			}

			rowItem->OnAcceptDragDrop = [node](GUI::SDragDropPackage* data)
			{
				CHierachyNode* dragNode = (CHierachyNode*)data->UserData;

				if (node->getTagDataType() == CHierachyNode::GameObject ||
					node->getTagDataType() == CHierachyNode::Container)
				{
					if (dragNode->getTagDataType() != CHierachyNode::Zone)
					{
						// dont accept drag zone to
						if (data->Name == "HierarchyNode")
							return true;
					}
				}
				else if (node->getTagDataType() == CHierachyNode::Zone)
				{

				}
				else if (node->getTagDataType() == CHierachyNode::Scene)
				{

				}
				return false;
			};

			rowItem->OnDragDropHover = [rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				if (node->getTagDataType() == CHierachyNode::Zone)
				{

				}
				if (node->getTagDataType() == CHierachyNode::Container)
				{
					GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
					if (local.Y < rowItem->height() * 0.25f)
					{
						rowItem->enableDrawLine(true, false);
					}
					else if (local.Y > rowItem->height() * 0.75f)
					{
						rowItem->enableDrawLine(false, true);
					}
					else
					{
						rowItem->enableDrawLine(false, false);
					}
				}
				else if (node->getTagDataType() == CHierachyNode::GameObject)
				{
					GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
					if (local.Y < rowItem->height() * 0.5f)
					{
						rowItem->enableDrawLine(true, false);
					}
					else
					{
						rowItem->enableDrawLine(false, true);
					}
				}
			};

			rowItem->OnDragDropOut = [rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				rowItem->enableDrawLine(false, false);
			};

			rowItem->OnDrop = [controller = this, rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				CHierachyNode* dragNode = (CHierachyNode*)data->UserData;

				if (node->getTagDataType() == CHierachyNode::Zone)
				{

				}
				else if (node->getTagDataType() == CHierachyNode::Container)
				{
					GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
					if (local.Y < rowItem->height() * 0.25f)
					{
						// drop on top
						controller->move(dragNode, node, true);
					}
					else if (local.Y > rowItem->height() * 0.75f)
					{
						// drop on bottom
						controller->move(dragNode, node, false);
					}
					else
					{
						// drop in child
						controller->moveToChild(dragNode, node);
					}
				}
				else if (node->getTagDataType() == CHierachyNode::GameObject)
				{
					GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
					if (local.Y < rowItem->height() * 0.5f)
					{
						// drop top
						controller->move(dragNode, node, true);
					}
					else
					{
						// drop bottom
						controller->move(dragNode, node, false);
					}
				}

				rowItem->enableDrawLine(false, false);
			};
		}

		void CHierarchyController::move(CHierachyNode* from, CHierachyNode* target, bool before)
		{
			if (from->getParent() == target->getParent())
			{
				// same parent
			}
			else
			{

			}
		}

		void CHierarchyController::moveToChild(CHierachyNode* from, CHierachyNode* target)
		{
			if (from->getParent() == target)
			{

			}
			else
			{

			}
		}
	}
}