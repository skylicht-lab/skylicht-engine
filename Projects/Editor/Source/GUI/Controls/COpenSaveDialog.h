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

#include "CBase.h"
#include "CDialogWindow.h"
#include "CDataGridView.h"
#include "CTextBox.h"
#include "CBoxLayout.h"
#include "CMenu.h"
#include "CCheckBox.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class COpenSaveDialog : public CDialogWindow
			{
			public:
				enum EDialogType
				{
					Open = 0,
					Save,
					SaveAs
				};

			protected:
				CDataGridView* m_files;

				CButton* m_back;
				CButton* m_next;
				CButton* m_up;
				CButton* m_refresh;

				CTextBox* m_path;
				CTextBox* m_search;
				CTextBox* m_fileName;

				CMenu* m_menuSetting;
				CMenu* m_menuFilter;

				EDialogType m_type;

				std::string m_root;
				std::string m_folder;
				std::string m_currentFolder;

				std::vector<std::string> m_filter;
				std::vector<std::string> m_backHistory;
				std::vector<std::string> m_nextHistory;

			public:
				COpenSaveDialog(CBase* base, EDialogType type, const char* root, const char* folder, const char* filter = "*.xml;*.*");

				virtual ~COpenSaveDialog();

				Listener OnSave;
				Listener OnOpen;
				Listener OnCancel;

			protected:

				void addCheckItemOnMenu(CBoxLayout* menu, const wchar_t* name, bool isCheck, Listener onCheck);

				std::string getRelativePath(const char* folder);

				void browseFolder(const char* folder, bool addHistory = true);

				void onClickFile(CBase* base);
				void onDbClickFile(CBase* base);

				void onBtnBack(CBase* base);
				void onBtnNext(CBase* base);
				void onBtnUp(CBase* base);
				void onBtnRefresh(CBase* base);

				void onSaveOpen(CBase* base);
				void onCancel(CBase* base);

				void onBtnFilter(CBase* base);
				void onBtnSetting(CBase* base);
			};
		}
	}
}