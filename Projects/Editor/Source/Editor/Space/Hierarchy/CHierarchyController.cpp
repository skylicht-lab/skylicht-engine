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

#include "pch.h"
#include "CHierarchyController.h"

namespace Skylicht
{
	namespace Editor
	{
		CHierarchyController::CHierarchyController(GUI::CCanvas* canvas, GUI::CTreeControl* tree) :
			m_canvas(canvas),
			m_tree(tree),
			m_node(NULL)
		{

		}

		CHierarchyController::~CHierarchyController()
		{

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

			// link data gui to node
			guiNode->tagData(node);

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

			return guiNode;
		}

		GUI::CTreeNode* CHierarchyController::add(CHierachyNode* node)
		{
			CHierachyNode* parent = node->getParent();
			GUI::CTreeNode* parentGuiNode = parent->getGUINode();

			return buildHierarchyNode(parentGuiNode, node);
		}
	}
}