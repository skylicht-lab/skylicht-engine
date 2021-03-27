/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

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
#include "CContextMenuAdd.h"

namespace Skylicht
{
	namespace Editor
	{
		CContextMenuAdd::CContextMenuAdd(GUI::CCanvas* canvas, CListFSController* listFSController) :
			m_canvas(canvas),
			m_listFSController(listFSController)
		{
			m_contextMenu = new GUI::CMenu(canvas);
			m_contextMenu->setHidden(true);
			m_contextMenu->OnCommand = BIND_LISTENER(&CContextMenuAdd::OnCommand, this);

			m_contextMenu->addItem(L"Folder");
			m_contextMenu->addSeparator();
			m_contextMenu->addItem(L"Scene");
			m_contextMenu->addItem(L"Material");
			m_contextMenu->addItem(L"Shader");
			m_contextMenu->addItem(L"GUI");
			m_contextMenu->addItem(L"Sprite");
			m_contextMenu->addItem(L"Animation");
		}

		CContextMenuAdd::~CContextMenuAdd()
		{

		}

		void CContextMenuAdd::popupMenu(GUI::CButton* button)
		{
			m_canvas->closeMenu();

			GUI::SPoint p = button->localPosToCanvas();
			m_contextMenu->open(GUI::SPoint(p.X, p.Y + button->height()));
		}

		void CContextMenuAdd::OnCommand(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"Folder")
			{
				m_listFSController->newFolder(m_listFSController->getCurrentFolder().c_str());
			}
		}
	}
}