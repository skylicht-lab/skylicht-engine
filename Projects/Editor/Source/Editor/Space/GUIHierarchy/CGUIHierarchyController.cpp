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
#include "CGUIHierarchyController.h"
#include "Utils/CStringImp.h"
#include "Utils/CPath.h"

#include "GameObject/CZone.h"
#include "Scene/CScene.h"

#include "Editor/SpaceController/CGUIDesignController.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIHierarchyController::CGUIHierarchyController(GUI::CCanvas* canvas, GUI::CTreeControl* tree, CEditor* editor) :
			m_editor(editor),
			m_tree(tree),
			m_node(NULL)
		{
			m_tree->OnKeyPress = std::bind(
				&CGUIHierarchyController::OnKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);

			m_tree->addAccelerator("Ctrl + C", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + C"); });
			m_tree->addAccelerator("Ctrl + V", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + V"); });
			m_tree->addAccelerator("Ctrl + D", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + D"); });
			m_tree->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
			m_tree->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });
		}

		CGUIHierarchyController::~CGUIHierarchyController()
		{
			if (m_node != NULL)
				m_node->nullGUI();
		}

		void CGUIHierarchyController::deleteHierarchyData()
		{
			if (m_node != NULL)
			{
				m_node->nullGUI();
				delete m_node;
				m_node = NULL;
			}
		}

		void CGUIHierarchyController::setTreeNode(CGUIHierachyNode* node)
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
			GUI::CTreeNode* root0 = buildTreeNode(m_tree, m_node);

			// expand tree
			root0->expand(false);
			for (GUI::CTreeNode* root1 : root0->getChildNodes())
				root1->expand(false);
		}

		CGUIHierachyNode* CGUIHierarchyController::getNodeByObject(CGUIElement* object)
		{
			if (m_node == NULL)
				return NULL;

			return m_node->getNodeByTag(object);
		}

		void CGUIHierarchyController::updateTreeNode(CGUIElement* object)
		{
			if (m_node == NULL)
				return;

			CGUIHierachyNode* node = m_node->getNodeByTag(object);
			if (node != NULL && node->OnUpdate != NULL)
			{
				node->OnUpdate(node);

				// rebuild-gui child of entity
				std::vector<CGUIHierachyNode*>& childs = node->getChilds();
				for (CGUIHierachyNode* child : childs)
				{
					if (child->getTagDataType() == CGUIHierachyNode::GUIElement)
						buildTreeNode(node->getGUINode(), child);
				}
			}
		}

		GUI::CTreeNode* CGUIHierarchyController::buildTreeNode(GUI::CTreeNode* parentGuiNode, CGUIHierachyNode* node)
		{
			// add node
			GUI::CTreeNode* guiNode = parentGuiNode->addNode(node->getName(), node->getIcon());

			// apply select event
			guiNode->OnSelectChange = BIND_LISTENER(&CGUIHierarchyController::OnSelectChange, this);

			// link data gui to node
			guiNode->tagData(node);

			// set drag drop data
			initDragDrop(guiNode, node);

			// link data node to gui
			node->setGUINode(guiNode);

			// apply active color
			if (node->haveColor())
			{
				guiNode->getRowItem()->setColor(node->getBGColor());
				guiNode->getRowItem()->enableDrawBackground(true);
			}
			else
			{
				guiNode->getRowItem()->enableDrawBackground(false);
			}

			// loop all childs
			std::vector<CGUIHierachyNode*>& childs = node->getChilds();
			for (CGUIHierachyNode* child : childs)
			{
				buildTreeNode(guiNode, child);
			}

			// expand
			parentGuiNode->expand(false);

			return guiNode;
		}

		GUI::CTreeNode* CGUIHierarchyController::addToTreeNode(CGUIHierachyNode* node)
		{
			CGUIHierachyNode* parent = node->getParent();
			GUI::CTreeNode* parentGuiNode = parent->getGUINode();

			GUI::CTreeNode* ret = buildTreeNode(parentGuiNode, node);
			m_tree->deselectAll();
			m_tree->getScrollControl()->scrollToItem(ret);

			ret->setSelected(true);
			return ret;
		}

		void CGUIHierarchyController::OnHotkey(GUI::CBase* base, const std::string& hotkey)
		{
			if (hotkey == "Ctrl + C")
			{
				CGUIDesignController::getInstance()->onCopy();
			}
			else if (hotkey == "Ctrl + V")
			{
				CGUIDesignController::getInstance()->onPaste();
			}
			else if (hotkey == "Ctrl + D")
			{
				CGUIDesignController::getInstance()->onDuplicate();
			}
			else if (hotkey == "Ctrl + Z")
			{
			}
			else if (hotkey == "Ctrl + Y")
			{
			}
		}

		void CGUIHierarchyController::OnKeyPress(GUI::CBase* control, int key, bool press)
		{
			GUI::CTreeControl* tree = dynamic_cast<GUI::CTreeControl*>(control);
			if (tree == NULL)
				return;

			if (press = true)
			{
				if (key == GUI::KEY_F2)
				{
					rename(tree->getChildSelected());
				}
				else if (key == GUI::KEY_DELETE)
				{
					CGUIDesignController::getInstance()->onDelete();
				}
			}
		}

		void CGUIHierarchyController::rename(GUI::CTreeNode* node)
		{
			if (node != NULL)
			{
				m_renameNode = node;
				m_renameNode = node;

				node->getTextEditHelper()->beginEdit(
					BIND_LISTENER(&CGUIHierarchyController::OnRename, this),
					BIND_LISTENER(&CGUIHierarchyController::OnCancelRename, this)
				);
			}
		}

		void CGUIHierarchyController::OnRename(GUI::CBase* control)
		{
			GUI::CTextBox* textbox = dynamic_cast<GUI::CTextBox*>(control);

			std::wstring newNameW = textbox->getString();

			CGUIHierachyNode* node = (CGUIHierachyNode*)m_renameNode->getTagData();
			node->setName(newNameW.c_str());

			if (node->OnUpdate != nullptr)
				node->OnUpdate(node);

			m_tree->focus();
		}

		void CGUIHierarchyController::OnCancelRename(GUI::CBase* control)
		{
			m_tree->focus();
		}

		void CGUIHierarchyController::OnSelectChange(GUI::CBase* control)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(control);
			if (treeNode != NULL)
			{
				CGUIHierachyNode* node = (CGUIHierachyNode*)treeNode->getTagData();
				if (node->OnSelected != nullptr)
					node->OnSelected(node, treeNode->isSelected());
			}
		}

		void CGUIHierarchyController::initDragDrop(GUI::CTreeNode* guiNode, CGUIHierachyNode* node)
		{
			GUI::CTreeRowItem* rowItem = guiNode->getRowItem();

			bool allowDrag = true;

			if (node == m_node)
				allowDrag = false;

			if (allowDrag)
			{
				GUI::SDragDropPackage* dragDrop = rowItem->setDragDropPackage("GUIHierarchyNode", node);
				dragDrop->DrawControl = guiNode->getTextItem();
			}

			rowItem->OnAcceptDragDrop = [node](GUI::SDragDropPackage* data)
			{
				if (data->Name == "GUIHierarchyNode")
				{
					return true;
				}
				else if (data->Name == "ListFSItem")
				{
					GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
					bool isFolder = rowItem->getTagBool();
					if (isFolder)
						return false;

					std::string path = rowItem->getTagString();
					std::string fileExt = CPath::getFileNameExt(path);
					fileExt = CStringImp::toLower(fileExt);
					if (fileExt == "png" || fileExt == "tga")
					{
						return true;
					}
				}
				return false;
			};

			rowItem->OnDragDropHover = [rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));

				if (node->getParent() == NULL)
				{
					// this is canvas node
					rowItem->enableDrawLine(false, false);
				}
				else
				{
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
			};

			rowItem->OnDragDropOut = [rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				rowItem->enableDrawLine(false, false);
			};

			rowItem->OnDrop = [&, editor = m_editor, rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				if (data->Name == "GUIHierarchyNode")
				{
					CGUIHierachyNode* dragNode = (CGUIHierachyNode*)data->UserData;

					if (node->getParent() == NULL)
					{
						// this is canvas node
						moveToChild(dragNode, node);
					}
					else
					{
						// this is child node
						GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
						if (local.Y < rowItem->height() * 0.25f)
						{
							move(dragNode, node, false);
						}
						else if (local.Y > rowItem->height() * 0.75f)
						{
							move(dragNode, node, true);
						}
						else
						{
							moveToChild(dragNode, node);
						}
					}
				}

				rowItem->enableDrawLine(false, false);
				editor->refresh();
			};
		}

		void CGUIHierarchyController::move(CGUIHierachyNode* from, CGUIHierachyNode* target, bool behind)
		{
			// remove parent gui
			bool isExpand = from->getGUINode()->isExpand();

			from->removeGUI();
			from->nullGUI();

			// update position
			from->bringNextNode(target, behind);

			// add new tree gui at new position
			GUI::CTreeNode* gui = buildTreeNode(target->getParent()->getGUINode(), from);

			// move gui to near target
			gui->bringNextToControl(target->getGUINode(), behind);

			if (isExpand)
				gui->expand(false);
			else
				gui->collapse(false);

			// move game object
			CGUIElement* fromObject = (CGUIElement*)from->getTagData();
			CGUIElement* targetObject = (CGUIElement*)target->getTagData();

			CGUIElement* parent = targetObject->getParent();
			parent->bringToNext(fromObject, targetObject, behind);
		}

		void CGUIHierarchyController::moveToChild(CGUIHierachyNode* from, CGUIHierachyNode* target)
		{
			// remove parent gui
			bool isExpand = from->getGUINode()->isExpand();

			from->removeGUI();
			from->nullGUI();

			target->bringToChild(from);

			// add new tree item
			GUI::CTreeNode* gui = buildTreeNode(target->getGUINode(), from);

			if (isExpand)
				gui->expand(false);
			else
				gui->collapse(false);

			// move game object
			CGUIElement* fromObject = (CGUIElement*)from->getTagData();
			CGUIElement* targetObject = (CGUIElement*)target->getTagData();

			targetObject->bringToChild(fromObject);
		}
	}
}
