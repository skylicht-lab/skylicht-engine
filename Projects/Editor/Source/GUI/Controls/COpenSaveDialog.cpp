/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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

#include "pch.h"
#include "CToolbar.h"
#include "COpenSaveDialog.h"
#include "CTextBox.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			COpenSaveDialog::COpenSaveDialog(CBase* base, EDialogType type) :
				CDialogWindow(base, 0.0f, 0.0f, 760.0f, 400.0f)
			{
				switch (type)
				{
				case Open:
					setCaption(L"Open");
					break;
				case Save:
					setCaption(L"Save");
					break;
				case SaveAs:
					setCaption(L"Save As");
					break;
				}

				CBase* topPanel = new CBase(this);
				topPanel->setHeight(42.0f);
				topPanel->dock(EPosition::Top);
				topPanel->enableRenderFillRect(true);
				topPanel->setFillRectColor(CThemeConfig::OpenSaveDialogTop);

				CToolbar* toolbar = new CToolbar(topPanel);
				toolbar->setMargin(SMargin(0.0f, 8.0f, 0.0f, 0.0f));
				toolbar->enableRenderFillRect(false);

				CButton* btn;
				SMargin buttonMargin(0.0f, 0.0f, 2.0f, 0.0f);

				btn = toolbar->addButton(ESystemIcon::Back);
				btn->enableDrawBackground(true);
				btn->setMargin(SMargin(10.0f, 0.0f, 2.0f, 0.0f));

				btn = toolbar->addButton(ESystemIcon::Next);
				btn->enableDrawBackground(true);
				btn->setMargin(buttonMargin);

				btn = toolbar->addButton(ESystemIcon::UpToParent);
				btn->enableDrawBackground(true);
				btn->setMargin(buttonMargin);

				btn = toolbar->addButton(ESystemIcon::Refresh);
				btn->enableDrawBackground(true);

				toolbar->addSpace(10.0f);

				btn = toolbar->addButton(ESystemIcon::NewFolder);
				btn->enableDrawBackground(true);

				toolbar->addSpace(10.0f);

				CTextBox* textbox = (CTextBox*)toolbar->addControl(new CTextBox(toolbar));
				textbox->setWidth(285.0f);
				textbox->setWrapMultiline(false);

				toolbar->addSpace(10.0f);

				textbox = (CTextBox*)toolbar->addControl(new CTextBox(toolbar));
				textbox->setWidth(150.0f);
				textbox->setWrapMultiline(false);
				textbox->showIcon();

				btn = toolbar->addButton(ESystemIcon::Setting, true);
				btn->enableDrawBackground(true);
				btn->setMargin(SMargin(0.0f, 0.0f, 10.0f, 0.0f));

				btn = toolbar->addButton(ESystemIcon::Filter, true);
				btn->enableDrawBackground(true);
				btn->setMargin(SMargin(0.0f, 0.0f, 10.0f, 0.0f));

				CBase* bottomPanel = new CBase(this);
				bottomPanel->setHeight(40.0f);
				bottomPanel->dock(EPosition::Bottom);
				bottomPanel->enableRenderFillRect(true);
				bottomPanel->setFillRectColor(CThemeConfig::OpenSaveDialogBottom);

				CBase* bottom = new CBase(bottomPanel);
				bottom->dock(EPosition::Top);
				bottom->setHeight(20.0f);
				bottom->setMargin(SMargin(0.0f, 10.0f, 0.0f, 0.0f));

				CButton* cancel = new CButton(bottom);
				cancel->setMargin(SMargin(0.0f, 0.0f, 10.0f, 0.0f));
				cancel->setLabel(L"Cancel");
				cancel->setWidth(130.0f);
				cancel->setTextAlignment(ETextAlign::TextCenter);
				cancel->dock(EPosition::Right);

				CButton* save = new CButton(bottom);
				save->setMargin(SMargin(0.0f, 0.0f, 5.0f, 0.0f));
				save->setLabel(L"Save");
				save->setWidth(130.0f);
				save->setTextAlignment(ETextAlign::TextCenter);
				save->dock(EPosition::Right);

				CTextBox* filename = new CTextBox(bottom);
				filename->dock(EPosition::Fill);
				filename->setMargin(SMargin(10.0f, 0.0f, 10.0f, 0.0f));

				m_files = new CDataGridView(this, 3);
				m_files->dock(EPosition::Fill);
				m_files->getHeader()->setLabel(0, L"Name");
				m_files->getHeader()->setLabel(1, L"Date Modified");
				m_files->getHeader()->setLabel(2, L"Size");

				m_files->setColumnWidth(this, 0, 400.0f);

				for (int i = 0; i < 5; i++)
				{
					CDataRowView* row = m_files->addItem(L"File", ESystemIcon::Folder);
					row->setIconColor(CThemeConfig::FolderColor);
					row->setLabel(1, L"00/00/0000 - 00:00:00");
					row->setLabel(2, L"1024 bytes");
				}

				setResizable(true);
				setCenterPosition();
			}

			COpenSaveDialog::~COpenSaveDialog()
			{

			}
		}
	}
}