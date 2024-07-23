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
#include "CContextMenuGUIElement.h"
#include "CGUIDesignController.h"
#include "Selection/CSelection.h"
#include "Editor/SpaceController/CPropertyController.h"
#include "GUI/Input/CInput.h"
#include "Entity/CEntityHandleData.h"

namespace Skylicht
{
	namespace Editor
	{
		CContextMenuGUIElement::CContextMenuGUIElement(GUI::CCanvas* canvas) :
			m_canvas(canvas),
			m_spaceHierarchy(NULL),
			m_contextNode(NULL)
		{
			m_contextMenu = new GUI::CMenu(canvas);
			m_contextMenu->setHidden(true);
			m_contextMenu->OnCommand = BIND_LISTENER(&CContextMenuGUIElement::OnContextMenuCommand, this);

			m_contextMenu->addItem(L"Rename", L"F2");
			m_contextMenu->addSeparator();
			m_items[0] = m_contextMenu->addItem(L"Copy", GUI::ESystemIcon::Copy, L"Ctrl + C");
			m_items[1] = m_contextMenu->addItem(L"Paste", GUI::ESystemIcon::Paste, L"Ctrl + V");
			m_items[2] = m_contextMenu->addItem(L"Duplicate", GUI::ESystemIcon::Duplicate, L"Ctrl + D");
			m_items[3] = m_contextMenu->addItem(L"Cut", L"Ctrl + X");
			m_contextMenu->addSeparator();
			m_items[4] = m_contextMenu->addItem(L"Delete", GUI::ESystemIcon::Trash);
			m_contextMenu->addSeparator();

			GUI::CMenu* addMenu = m_contextMenu->addItem(L"Add")->getMenu();
			addMenu->addItem(L"GUI Element");
			addMenu->addItem(L"GUI Rect");
			addMenu->addItem(L"GUI Image");
			addMenu->addItem(L"GUI Sprite");
			addMenu->addItem(L"GUI Text");
			addMenu->addItem(L"GUI Mask");
			addMenu->addItem(L"GUI Layout");
			addMenu->addItem(L"GUI Fit Sprite");
			addMenu->addSeparator();
			addMenu->addItem(L"UI Button");
			addMenu->addItem(L"UI Button (Shiny)");
			addMenu->addItem(L"UI Slider");
			addMenu->addItem(L"UI Input");
			addMenu->addItem(L"UI Checkbox");
			addMenu->addItem(L"UI Switch");
			addMenu->OnCommand = BIND_LISTENER(&CContextMenuGUIElement::OnContextMenuAddCommand, this);
		}

		CContextMenuGUIElement::~CContextMenuGUIElement()
		{

		}

		bool CContextMenuGUIElement::onContextMenu(CSpaceGUIHierarchy* spaceHierachy, CGUIHierachyNode* node)
		{
			GUI::CInput* input = GUI::CInput::getInput();
			GUI::SPoint mousePos = input->getMousePosition();

			m_contextNode = node;
			m_spaceHierarchy = spaceHierachy;
			m_canvas->closeMenu();

			for (int i = 0; i < 5; i++)
			{
				if (m_contextNode->getTagDataType() == CGUIHierachyNode::Canvas)
					m_items[i]->setDisabled(true);
				else
					m_items[i]->setDisabled(false);
			}

			m_contextMenu->open(mousePos);

			return true;
		}

		void CContextMenuGUIElement::OnContextMenuCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL || m_contextNode == NULL)
				return;

			CGUIElement* node = (CGUIElement*)m_contextNode->getTagData();

			const std::wstring& command = menuItem->getLabel();

			if (command == L"Delete")
			{
				CSelection::getInstance()->unSelect(node);
				CPropertyController::getInstance()->setProperty(NULL);

				node->remove();
				m_contextNode->remove();
				m_contextNode = NULL;
			}
			else if (command == L"Copy")
			{
				CGUIDesignController::getInstance()->onCopy();
			}
			else if (command == L"Paste")
			{
				CGUIDesignController::getInstance()->onPaste();
			}
			else if (command == L"Duplicate")
			{
				CGUIDesignController::getInstance()->onDuplicate();
			}
			else if (command == L"Cut")
			{
				CGUIDesignController::getInstance()->onCut();
			}
		}

		void CContextMenuGUIElement::OnContextMenuAddCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL || m_contextNode == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();
			CGUIDesignController::getInstance()->createGUINode(m_contextNode, command);
		}
	}
}
