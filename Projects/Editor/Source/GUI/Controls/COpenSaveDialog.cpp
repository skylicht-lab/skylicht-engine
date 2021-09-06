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

#include <filesystem>
#include <chrono>
#include <sstream>
#include <sys/stat.h>

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			COpenSaveDialog::COpenSaveDialog(CBase* base, EDialogType type, const char* folder, const char* filter) :
				CDialogWindow(base, 0.0f, 0.0f, 760.0f, 400.0f),
				m_type(type),
				m_folder(folder)
			{
				CStringImp::splitString(filter, ";", m_filter);

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

				m_back = toolbar->addButton(ESystemIcon::Back);
				m_back->enableDrawBackground(true);
				m_back->setMargin(SMargin(10.0f, 0.0f, 2.0f, 0.0f));
				m_back->setDisabled(true);

				m_next = toolbar->addButton(ESystemIcon::Next);
				m_next->enableDrawBackground(true);
				m_next->setMargin(buttonMargin);
				m_next->setDisabled(true);

				m_up = toolbar->addButton(ESystemIcon::UpToParent);
				m_up->enableDrawBackground(true);
				m_up->setMargin(buttonMargin);
				m_up->setDisabled(true);

				m_refresh = toolbar->addButton(ESystemIcon::Refresh);
				m_refresh->enableDrawBackground(true);

				toolbar->addSpace(10.0f);

				m_newfolder = toolbar->addButton(ESystemIcon::NewFolder);
				m_newfolder->enableDrawBackground(true);

				toolbar->addSpace(10.0f);

				m_path = (CTextBox*)toolbar->addControl(new CTextBox(toolbar));
				m_path->setWidth(285.0f);
				m_path->setWrapMultiline(false);

				toolbar->addSpace(10.0f);

				m_search = (CTextBox*)toolbar->addControl(new CTextBox(toolbar));
				m_search->setWidth(150.0f);
				m_search->setWrapMultiline(false);
				m_search->showIcon();

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
				cancel->OnPress = BIND_LISTENER(&COpenSaveDialog::onCancel, this);

				CButton* action = new CButton(bottom);
				action->setMargin(SMargin(0.0f, 0.0f, 5.0f, 0.0f));
				if (type == COpenSaveDialog::Save || type == COpenSaveDialog::SaveAs)
					action->setLabel(L"Save");
				else if (type == COpenSaveDialog::Open)
					action->setLabel(L"Open");
				action->setWidth(130.0f);
				action->setTextAlignment(ETextAlign::TextCenter);
				action->dock(EPosition::Right);
				action->OnPress = BIND_LISTENER(&COpenSaveDialog::onSaveOpen, this);

				m_fileName = new CTextBox(bottom);
				m_fileName->dock(EPosition::Fill);
				m_fileName->setMargin(SMargin(10.0f, 0.0f, 10.0f, 0.0f));

				m_files = new CDataGridView(this, 3);
				m_files->dock(EPosition::Fill);
				m_files->getHeader()->setLabel(0, L"Name");
				m_files->getHeader()->setLabel(1, L"Date Modified");
				m_files->getHeader()->setLabel(2, L"Size");

				m_files->setColumnWidth(this, 0, 400.0f);

				browseFolder(m_folder.c_str());

				setResizable(true);
				setCenterPosition();
			}

			COpenSaveDialog::~COpenSaveDialog()
			{

			}

			void COpenSaveDialog::browseFolder(const char* folder)
			{
				m_currentFolder = folder;

				m_files->removeAllItem();

				wchar_t text[512];

				for (const auto& file : fs::directory_iterator(m_currentFolder))
				{
					std::string path = file.path().generic_u8string();
					std::string fileName = CPath::getFileName(path);

					CDataRowView* row = NULL;

					if (file.is_directory())
					{
						row = m_files->addItem(CStringImp::convertUTF8ToUnicode(fileName.c_str()).c_str(), ESystemIcon::Folder);
						row->setIconColor(CThemeConfig::FolderColor);
					}
					else
					{
						row = m_files->addItem(CStringImp::convertUTF8ToUnicode(fileName.c_str()).c_str(), ESystemIcon::File);

						uintmax_t fileSize = fs::file_size(file);
						if (fileSize < 1024)
							swprintf(text, 512, L"%d bytes", (int)fileSize);
						else if (fileSize < 1024 * 1024)
							swprintf(text, 512, L"%d kb", (int)(fileSize / 1024));
						else if (fileSize < 1024 * 1024 * 1024)
							swprintf(text, 512, L"%d mb", (int)(fileSize / (1024 * 1024)));
						else
							swprintf(text, 512, L"%d gb", (int)(fileSize / (1024 * 1024 * 1024)));

						row->setLabel(2, text);
					}

					struct stat result;
					if (stat(path.c_str(), &result) == 0)
					{
						time_t modifyTime = result.st_mtime;
						tm* timeinfo = gmtime(&modifyTime);

						const wchar_t* monthString[] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };

						swprintf(text, 512, L"%02d %s %d %02d:%02d",
							timeinfo->tm_mday,
							monthString[timeinfo->tm_mon],
							timeinfo->tm_year + 1900,
							timeinfo->tm_hour,
							timeinfo->tm_min);

						row->setLabel(1, text);
					}
				}
			}

			void COpenSaveDialog::onBtnBack(CBase* base)
			{

			}

			void COpenSaveDialog::onBtnNext(CBase* base)
			{

			}

			void COpenSaveDialog::onBtnRefresh(CBase* base)
			{

			}

			void COpenSaveDialog::onBtnNewFolder(CBase* base)
			{

			}

			void COpenSaveDialog::onSaveOpen(CBase* base)
			{
				if (m_type == COpenSaveDialog::Save || m_type == COpenSaveDialog::SaveAs)
				{
					if (OnSave != nullptr)
						OnSave(this);
				}
				else
				{
					if (OnOpen != nullptr)
						OnOpen(this);
				}

				onCloseWindow();
			}

			void COpenSaveDialog::onCancel(CBase* base)
			{
				if (OnCancel != nullptr)
					OnCancel(this);
				onCloseWindow();
			}
		}
	}
}