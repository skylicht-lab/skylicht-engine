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

#include "pch.h"
#include "CParticleHierarchyController.h"

namespace Skylicht
{
	namespace Editor
	{
		CParticleHierarchyController::CParticleHierarchyController(GUI::CCanvas* canvas, GUI::CTreeControl* tree, CEditor* editor) :
			m_tree(tree),
			m_editor(editor),
			m_node(NULL)
		{

		}

		CParticleHierarchyController::~CParticleHierarchyController()
		{

		}

		void CParticleHierarchyController::setTreeNode(CParticleHierachyNode* node)
		{
			if (m_node != node)
			{
				if (m_node != NULL)
					m_node->nullGUI();
			}

			m_node = node;

			// remove all node
			m_tree->removeAllTreeNode();

			if (!m_node)
				return;

			// add child nodes
			GUI::CTreeNode* root0 = buildTreeNode(m_tree, m_node);

			// expand tree
			root0->expand(false);
			for (GUI::CTreeNode* root1 : root0->getChildNodes())
				root1->expand(false);
		}

		GUI::CTreeNode* CParticleHierarchyController::buildTreeNode(GUI::CTreeNode* parentGuiNode, CParticleHierachyNode* node)
		{
			// add node
			GUI::CTreeNode* guiNode = parentGuiNode->addNode(node->getName(), node->getIcon());

			// apply select event
			guiNode->OnSelectChange = BIND_LISTENER(&CParticleHierarchyController::OnSelectChange, this);

			// link data gui to node
			guiNode->tagData(node);

			// set drag drop data
			// initDragDrop(guiNode, node);

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
			std::vector<CParticleHierachyNode*>& childs = node->getChilds();
			for (CParticleHierachyNode* child : childs)
			{
				buildTreeNode(guiNode, child);
			}

			// expand
			parentGuiNode->expand(false);

			return guiNode;
		}

		GUI::CTreeNode* CParticleHierarchyController::addToTreeNode(CParticleHierachyNode* node)
		{
			CParticleHierachyNode* parent = node->getParent();
			GUI::CTreeNode* parentGuiNode = parent->getGUINode();

			GUI::CTreeNode* ret = buildTreeNode(parentGuiNode, node);
			m_tree->deselectAll();
			m_tree->getScrollControl()->scrollToItem(ret);

			ret->setSelected(true);
			return ret;
		}

		void CParticleHierarchyController::OnSelectChange(GUI::CBase* control)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(control);
			if (treeNode != NULL)
			{
				CParticleHierachyNode* node = (CParticleHierachyNode*)treeNode->getTagData();
				if (node->OnSelected != nullptr)
				{
					node->OnSelected(node, treeNode->isSelected());
				}
			}
		}
	}
}