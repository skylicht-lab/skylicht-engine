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
#include "Utils/CPath.h"

#include "GameObject/CZone.h"
#include "Scene/CScene.h"

#include "Editor/SpaceController/CSceneController.h"
#include "AssetManager/CAssetManager.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CHierarchyController::CHierarchyController(GUI::CCanvas* canvas, GUI::CTreeControl* tree, CEditor* editor) :
			m_editor(editor),
			m_canvas(canvas),
			m_tree(tree),
			m_node(NULL),
			m_renameNode(NULL)
		{
			m_tree->OnKeyPress = std::bind(
				&CHierarchyController::OnKeyPress,
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

			m_tree->addAccelerator("F", [&](GUI::CBase* base) {this->OnHotkey(base, "F"); });

			tree->getInnerPanel()->OnAcceptDragDrop = [](GUI::SDragDropPackage* data)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
						bool isFolder = rowItem->getTagBool();
						if (isFolder)
							return false;

						std::string path = rowItem->getTagString();
						std::string fileExt = CPath::getFileNameExt(path);
						fileExt = CStringImp::toLower(fileExt);
						if (fileExt == "dae" ||
							fileExt == "obj" ||
							fileExt == "fbx" ||
							fileExt == "smesh" ||
							fileExt == "template" ||
							fileExt == "particle")
						{
							return true;
						}
					}
					return false;
				};

			tree->getInnerPanel()->OnDrop = [&, editor = m_editor](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* fsItem = (GUI::CListRowItem*)data->UserData;

						std::string path = fsItem->getTagString();
						std::string fileExt = CPath::getFileNameExt(path);
						fileExt = CStringImp::toLower(fileExt);

						CHierachyNode* newNode = NULL;
						CSceneController* sceneController = CSceneController::getInstance();

						CGameObject* targetObject = NULL;

						if (fileExt == "template")
						{
							CContainerObject* container = sceneController->getZone();

							targetObject = sceneController->createTemplateObject(path, container, false);
							updateTreeNode(targetObject);

							newNode = m_node->getNodeByTag(targetObject);
						}
						else
						{
							CContainerObject* container = sceneController->getZone();
							CHierachyNode* node = getNodeByObject(container);
							if (node)
							{
								newNode = createChildObject(node);
								if (newNode != NULL)
								{
									targetObject = (CGameObject*)newNode->getTagData();

									CSceneController* sceneController = CSceneController::getInstance();
									sceneController->createResourceComponent(path, targetObject);

									updateTreeNode(targetObject);
									newNode->getGUINode()->setSelected(true);
								}
							}
						}

						if (targetObject)
							sceneController->getHistory()->saveCreateHistory(targetObject);
					}

					editor->refresh();
				};
		}

		CHierarchyController::~CHierarchyController()
		{
			if (m_node != NULL)
			{
				m_node->nullGUI();
				delete m_node;
			}
		}

		void CHierarchyController::deleteHierarchyData()
		{
			if (m_node != NULL)
			{
				m_node->nullGUI();
				delete m_node;
				m_node = NULL;
			}
		}

		void CHierarchyController::setTreeNode(CHierachyNode* node)
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

		CHierachyNode* CHierarchyController::getNodeByObject(CGameObject* object)
		{
			if (m_node == NULL)
				return NULL;

			return m_node->getNodeByTag(object);
		}

		void CHierarchyController::updateObjectToUI(CGameObject* object, CHierachyNode* node)
		{
			// update node name
			node->setName(object->getName());

			// visible
			if (object->isVisible())
			{
				// enable
				if (object->isEnable())
					node->setTextColor(GUI::SGUIColor(255, 250, 250, 250));
				else
					node->setTextColor(GUI::SGUIColor(255, 150, 150, 200));
			}
			else
				node->setTextColor(GUI::SGUIColor(255, 150, 150, 150));

			// show subicon?
			if (node->getTagDataType() == CHierachyNode::Scene ||
				node->getTagDataType() == CHierachyNode::Zone ||
				node->getTagDataType() == CHierachyNode::Entity)
				node->showSubIcon(false);
			else
				node->showSubIcon(true);

			// lock
			if (object->isLock())
				node->setSubIcon(GUI::ESystemIcon::Lock);
			else
				node->setSubIcon(GUI::ESystemIcon::None);

			// template object
			if (object->isTemplateAsset())
			{
				if (CAssetManager::getInstance()->isExist(object->getTemplateAsset()))
				{
					node->setIconColor(GUI::SGUIColor(255, 110, 170, 255));

					if (object->isTemplateChanged())
						node->setIconColor(GUI::SGUIColor(255, 170, 110, 255));
				}
				else
					node->setIconColor(GUI::SGUIColor(255, 255, 170, 110));
			}
			else
			{
				node->setIconColor(GUI::SGUIColor(255, 200, 200, 200));
			}
		}

		void CHierarchyController::updateTreeNode(CGameObject* object, bool rebuildAllTree)
		{
			if (m_node == NULL)
				return;

			CHierachyNode* node = m_node->getNodeByTag(object);
			if (node != NULL)
			{
				updateObjectToUI(object, node);

				// call sync node
				if (node->OnUpdate)
					node->OnUpdate(node);

				// rebuild-gui child of entity
				updateNodeToHierarchy(node, rebuildAllTree);
			}
		}

		void CHierarchyController::updateNodeToHierarchy(CHierachyNode* node, bool rebuildAllTree)
		{
			std::vector<CHierachyNode*>& childs = node->getChilds();
			for (CHierachyNode* child : childs)
			{
				if (rebuildAllTree)
				{
					buildTreeNode(node->getGUINode(), child);
				}
				else
				{
					if (child->getTagDataType() == CHierachyNode::Entity)
						buildTreeNode(node->getGUINode(), child);
				}
			}
		}

		GUI::CTreeNode* CHierarchyController::buildTreeNode(GUI::CTreeNode* parentGuiNode, CHierachyNode* node)
		{
			// add node
			GUI::CTreeNode* guiNode = parentGuiNode->addNode(node->getName(), node->getIcon());

			// apply select event
			guiNode->OnSelectChange = BIND_LISTENER(&CHierarchyController::OnSelectChange, this);
			guiNode->OnDoubleClick = BIND_LISTENER(&CHierarchyController::OnDoubleClick, this);

			// link data gui to node
			guiNode->tagData(node);

			// set drag drop data
			initDragDrop(guiNode, node);

			// link data node to gui
			node->setGUINode(guiNode);

			// set icon color
			guiNode->setIconColor(node->getIconColor());

			// text color
			guiNode->setLabelColor(node->getTextColor());

			// show sub icon
			guiNode->setSubIcon(node->getSubIcon());
			guiNode->getTextItem()->showSubIcon(node->haveSubIcon());

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
			std::vector<CHierachyNode*>& childs = node->getChilds();
			for (CHierachyNode* child : childs)
			{
				buildTreeNode(guiNode, child);
			}

			// expand
			// parentGuiNode->expand(false);

			return guiNode;
		}

		GUI::CTreeNode* CHierarchyController::addToTreeNode(CHierachyNode* node)
		{
			CHierachyNode* parent = node->getParent();
			GUI::CTreeNode* parentGuiNode = parent->getGUINode();

			GUI::CTreeNode* ret = buildTreeNode(parentGuiNode, node);
			m_tree->deselectAll();
			m_tree->getScrollControl()->scrollToItem(ret);

			ret->setSelected(true);
			return ret;
		}

		void CHierarchyController::OnHotkey(GUI::CBase* base, const std::string& hotkey)
		{
			if (hotkey == "Ctrl + C")
			{
				CSceneController::getInstance()->onCopy();
			}
			else if (hotkey == "Ctrl + V")
			{
				CSceneController::getInstance()->onPaste();
			}
			else if (hotkey == "Ctrl + D")
			{
				CSceneController::getInstance()->onDuplicate();
			}
			else if (hotkey == "Ctrl + Z")
			{
				CSceneController::getInstance()->onUndo();
			}
			else if (hotkey == "Ctrl + Y")
			{
				CSceneController::getInstance()->onRedo();
			}
			else if (hotkey == "F")
			{
				CSceneController::getInstance()->focusCameraToSelectObject();
			}
		}

		void CHierarchyController::OnKeyPress(GUI::CBase* control, int key, bool press)
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
					CSceneController::getInstance()->onDelete();
				}
			}
		}

		void CHierarchyController::rename(GUI::CTreeNode* node)
		{
			if (node != NULL)
			{
				m_renameNode = node;
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

			// rebuild-gui child of entity
			updateNodeToHierarchy(node, false);

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

		void CHierarchyController::OnDoubleClick(GUI::CBase* control)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(control);
			if (treeNode != NULL)
			{
				CHierachyNode* node = (CHierachyNode*)treeNode->getTagData();
				if (node->OnDoubleClick != nullptr)
					node->OnDoubleClick(node);
			}
		}

		void CHierarchyController::initDragDrop(GUI::CTreeNode* guiNode, CHierachyNode* node)
		{
			GUI::CTreeRowItem* rowItem = guiNode->getRowItem();

			bool allowDrag = true;

			if (node == m_node)
				allowDrag = false;

			if (node->isTagEntity())
				allowDrag = false;

			if (allowDrag)
			{
				GUI::SDragDropPackage* dragDrop = rowItem->setDragDropPackage("HierarchyNode", node);
				dragDrop->DrawControl = guiNode->getTextItem();
			}

			rowItem->OnAcceptDragDrop = [node](GUI::SDragDropPackage* data)
				{
					if (node->isTagGameObject())
					{
						// accept hierarchy node
						if (data->Name == "HierarchyNode")
						{
							CHierachyNode* dragNode = (CHierachyNode*)data->UserData;

							if (dragNode->getTagDataType() == CHierachyNode::Zone)
							{
								if (node->getTagDataType() == CHierachyNode::Zone)
								{
									// only allow zone drag over on zone
									return true;
								}
								return false;
							}
							else
							{
								// dont accept drag to child node
								if (dragNode->isChild(node))
									return false;
							}
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
							if (fileExt == "dae" ||
								fileExt == "obj" ||
								fileExt == "fbx" ||
								fileExt == "smesh" ||
								fileExt == "template")
							{
								return true;
							}
						}
					}
					return false;
				};

			rowItem->OnDragDropHover = [rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (node->getTagDataType() == CHierachyNode::Zone)
					{
						if (data->Name == "HierarchyNode")
						{
							CHierachyNode* dragNode = (CHierachyNode*)data->UserData;
							if (dragNode->getTagDataType() == CHierachyNode::Zone)
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
						}
						else if (data->Name == "ListFSItem")
						{
							rowItem->enableDrawLine(false, true);
						}
					}
					else if (node->getTagDataType() == CHierachyNode::Container)
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

			rowItem->OnDrop = [&, editor = m_editor, rowItem, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "HierarchyNode")
					{
						std::vector<CHierachyNode*> affectNodes;
						std::list<GUI::CTreeNode*> selectNodes = m_tree->getSelectedNodes();
						for (GUI::CTreeNode* node : selectNodes)
						{
							CHierachyNode* tagData = (CHierachyNode*)node->getTagData();
							if (tagData)
								affectNodes.push_back(tagData);
						}

						std::vector<CGameObject*> listGameObjects;
						for (CHierachyNode* dragNode : affectNodes)
						{
							CGameObject* gameObject = (CGameObject*)dragNode->getTagData();
							listGameObjects.push_back(gameObject);
						}

						CSceneHistory* history = CSceneController::getInstance()->getHistory();
						history->enableAddSelectHistory(false);

						for (CHierachyNode* dragNode : affectNodes)
						{
							if (node->getTagDataType() == CHierachyNode::Zone)
							{
								if (dragNode->getTagDataType() == CHierachyNode::Zone)
								{
									GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
									if (local.Y < rowItem->height() * 0.5f)
										move(dragNode, node, false);
									else
										move(dragNode, node, true);
								}
								else
								{
									moveToChild(dragNode, node);
								}
								dragNode->getGUINode()->setSelected(true);
							}
							else if (node->getTagDataType() == CHierachyNode::Container)
							{
								GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
								if (local.Y < rowItem->height() * 0.25f)
									move(dragNode, node, false);
								else if (local.Y > rowItem->height() * 0.75f)
									move(dragNode, node, true);
								else
									moveToChild(dragNode, node);

								dragNode->getGUINode()->setSelected(true);
							}
							else if (node->getTagDataType() == CHierachyNode::GameObject)
							{
								GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
								if (local.Y < rowItem->height() * 0.5f)
									move(dragNode, node, false);
								else
									move(dragNode, node, true);
								dragNode->getGUINode()->setSelected(true);
							}
						}

						// save the modify struct
						history->saveStructureHistory(listGameObjects);
						history->enableAddSelectHistory(true);
					}
					else if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* fsItem = (GUI::CListRowItem*)data->UserData;

						std::string path = fsItem->getTagString();
						std::string fileExt = CPath::getFileNameExt(path);
						fileExt = CStringImp::toLower(fileExt);

						CHierachyNode* newNode = NULL;

						if (fileExt == "template")
						{
							CContainerObject* container = NULL;
							bool behind = false;
							bool updateNodePosition = true;

							if (node->getTagDataType() == CHierachyNode::Zone)
							{
								container = (CContainerObject*)node->getTagData();
								updateNodePosition = false;
							}
							else if (node->getTagDataType() == CHierachyNode::Container)
							{
								GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
								if (local.Y < rowItem->height() * 0.25f)
								{
									container = (CContainerObject*)node->getParent()->getTagData();
									behind = false;
								}
								else if (local.Y > rowItem->height() * 0.75f)
								{
									container = (CContainerObject*)node->getParent()->getTagData();
									behind = true;
								}
								else
								{
									container = (CContainerObject*)node->getTagData();
									updateNodePosition = false;
								}
							}
							else if (node->getTagDataType() == CHierachyNode::GameObject)
							{
								GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
								if (local.Y < rowItem->height() * 0.5f)
								{
									container = (CContainerObject*)node->getParent()->getTagData();
									behind = false;
								}
								else
								{
									container = (CContainerObject*)node->getParent()->getTagData();
									behind = true;
								}
							}

							if (container)
							{
								CSceneController* sceneController = CSceneController::getInstance();

								CGameObject* targetObject = sceneController->createTemplateObject(path, container);
								updateTreeNode(targetObject);

								newNode = m_node->getNodeByTag(targetObject);
								if (newNode && updateNodePosition)
								{
									move(newNode, node, behind);
									newNode->getGUINode()->setSelected(true);
								}
							}
						}
						else
						{
							if (node->getTagDataType() == CHierachyNode::Zone)
							{
								newNode = createChildObject(node);
							}
							else if (node->getTagDataType() == CHierachyNode::Container)
							{
								GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
								if (local.Y < rowItem->height() * 0.25f)
								{
									newNode = createObjectAt(node, false);
								}
								else if (local.Y > rowItem->height() * 0.75f)
								{
									newNode = createObjectAt(node, true);
								}
								else
								{
									newNode = createChildObject(node);
								}
							}
							else if (node->getTagDataType() == CHierachyNode::GameObject)
							{
								GUI::SPoint local = rowItem->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));
								if (local.Y < rowItem->height() * 0.5f)
								{
									newNode = createObjectAt(node, false);
								}
								else
								{
									newNode = createObjectAt(node, true);
								}
							}

							if (newNode != NULL)
							{
								CGameObject* targetObject = (CGameObject*)newNode->getTagData();

								CSceneController* sceneController = CSceneController::getInstance();
								sceneController->createResourceComponent(path, targetObject);

								updateTreeNode(targetObject);
								newNode->getGUINode()->setSelected(true);
							}
						}
					}

					rowItem->enableDrawLine(false, false);
					editor->refresh();
				};
		}

		void CHierarchyController::move(CHierachyNode* from, CHierachyNode* target, bool behind)
		{
			if (!from->isTagGameObject() || !target->isTagGameObject())
				return;

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
			CGameObject* fromObject = (CGameObject*)from->getTagData();
			CGameObject* targetObject = (CGameObject*)target->getTagData();

			if (dynamic_cast<CZone*>(fromObject) != NULL &&
				dynamic_cast<CZone*>(targetObject) != NULL)
			{
				CScene* scene = fromObject->getScene();
				scene->bringToNext(dynamic_cast<CZone*>(fromObject), dynamic_cast<CZone*>(targetObject), behind);
			}
			else
			{
				CContainerObject* parent = (CContainerObject*)targetObject->getParent();
				parent->bringToNext(fromObject, targetObject, behind);
			}
		}

		void CHierarchyController::moveToChild(CHierachyNode* from, CHierachyNode* target)
		{
			if (!from->isTagGameObject() || !target->isTagGameObject())
				return;

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
			CGameObject* fromObject = (CGameObject*)from->getTagData();
			CGameObject* targetObject = (CGameObject*)target->getTagData();
			CContainerObject* parent = (CContainerObject*)targetObject;
			parent->bringToChild(fromObject);
		}

		CHierachyNode* CHierarchyController::createObjectAt(CHierachyNode* position, bool behind)
		{
			CHierachyNode* parentNode = position->getParent();

			CGameObject* targetObject = (CGameObject*)position->getTagData();
			CContainerObject* parentObject = (CContainerObject*)targetObject->getParent();

			CGameObject* newGameObject = CSceneController::getInstance()->createEmptyObject(parentObject);
			CHierachyNode* thisNode = parentNode->getNodeByTag(newGameObject);
			if (thisNode != NULL)
			{
				parentObject->bringToNext(newGameObject, targetObject, behind);
				thisNode->bringNextNode(position, behind);
				thisNode->getGUINode()->bringNextToControl(position->getGUINode(), behind);
			}

			return thisNode;
		}

		CHierachyNode* CHierarchyController::createChildObject(CHierachyNode* parent)
		{
			CGameObject* parentGameObject = (CGameObject*)parent->getTagData();
			CContainerObject* parentContainer = (CContainerObject*)parentGameObject;

			CGameObject* newGameObject = CSceneController::getInstance()->createEmptyObject(parentContainer, false);
			CHierachyNode* thisNode = parent->getNodeByTag(newGameObject);
			return thisNode;
		}
	}
}