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
#include "CSearchAssetController.h"
#include "AssetManager/CAssetManager.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CSearchAssetController::CSearchAssetController(GUI::CTextBox* textbox, GUI::CBase* searchInfo, GUI::CLabel* labelSearch, GUI::CButton* buttonCancel, CListFSController* listController) :
			m_inputSearch(textbox),
			m_searchInfo(searchInfo),
			m_labelSearch(labelSearch),
			m_buttonCancel(buttonCancel),
			m_inputTimeout(0.0f),
			m_changed(false),
			m_listFSController(listController)
		{
			m_inputSearch->OnTextChange = BIND_LISTENER(&CSearchAssetController::OnSearchChanged, this);

			m_buttonCancel->OnPress = BIND_LISTENER(&CSearchAssetController::OnCancelSearch, this);
		}

		CSearchAssetController::~CSearchAssetController()
		{

		}

		void CSearchAssetController::update()
		{
			if (m_changed)
			{
				m_inputTimeout = m_inputTimeout - getTimeStep();
				if (m_inputTimeout <= 0.0f)
				{
					m_inputTimeout = 0.0f;
					m_changed = false;

					if (m_searchString.empty())
					{
						// hide search ui
						m_searchInfo->setHidden(true);

						// refresh
						m_listFSController->refresh();
					}
					else
					{
						// begin search
						std::wstring text = L"Search in Assets: \"";
						text += m_searchString;
						text += L"\"";

						m_labelSearch->setString(text);
						m_labelSearch->sizeToContents();

						m_searchInfo->setHidden(false);

						search(m_searchString);
					}
				}
			}
		}

		void CSearchAssetController::OnSearchChanged(GUI::CBase* base)
		{
			m_inputTimeout = 500.0f;
			m_changed = true;
			m_searchString = m_inputSearch->getString();
		}

		void CSearchAssetController::OnCancelSearch(GUI::CBase* base)
		{
			m_inputTimeout = 0.0f;
			m_changed = true;
			m_searchString = L"";
			m_inputSearch->setString(m_searchString);

			m_listFSController->refresh();
		}

		void CSearchAssetController::hideSearchUI()
		{
			m_inputTimeout = 0.0f;
			m_changed = true;
			m_searchString = L"";
			m_inputSearch->setString(m_searchString);
		}

		void CSearchAssetController::search(const std::wstring& string)
		{
			std::vector<SFileInfo> files;

			std::string search = CStringImp::convertUnicodeToUTF8(string.c_str());
			if (search.size() >= 2)
				CAssetManager::getInstance()->search(search.c_str(), files);

			m_listFSController->add(m_listFSController->getCurrentFolder(), files);
		}
	}
}
