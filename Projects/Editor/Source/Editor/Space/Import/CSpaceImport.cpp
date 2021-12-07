/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "GUI/CGUIContext.h"
#include "CSpaceImport.h"
#include "AssetManager/CAssetManager.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceImport::CSpaceImport(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_progressBar(NULL),
			m_statusText(NULL),
			m_importer(NULL),
			m_state(None)
		{
			m_progressBar = new GUI::CProgressBar(window);
			m_progressBar->dock(GUI::EPosition::Top);
			m_progressBar->setMargin(GUI::SMargin(14.0f, 14.0, 14.0, 0.0f));

			m_statusText = new GUI::CLabel(window);
			m_statusText->dock(GUI::EPosition::Fill);
			m_statusText->setMargin(GUI::SMargin(14.0f, 5.0, 14.0, 0.0f));
			m_statusText->setWrapMultiline(true);
			m_statusText->setString(L"Importing...");
		}

		CSpaceImport::~CSpaceImport()
		{
			delete m_importer;
		}

		void CSpaceImport::initImportAll()
		{
			if (m_importer != NULL)
				delete m_importer;

			Editor::CAssetManager* assetManager = Editor::CAssetManager::getInstance();
			assetManager->discoveryAssetFolder();
			assetManager->update();

			m_importer = new CAssetImporter(assetManager->getListFiles());

			m_state = ImportAsset;
		}

		void CSpaceImport::initImportFiles(std::list<SFileNode*>& files, std::list<std::string>& deleted)
		{
			if (m_importer != NULL)
				delete m_importer;

			m_importer = new CAssetImporter(files);

			if (deleted.size() > 0)
				m_importer->addDeleted(deleted);

			m_state = ImportAsset;
		}

		void CSpaceImport::update()
		{
			if (m_state == ImportAsset)
			{
				float percent = 0.0f;
				std::string last;

				bool finish = m_importer->load(10);
				m_importer->getImportStatus(percent, last);

				m_progressBar->setPercent(percent);

				std::string status = "Importing...\n";
				status += last;
				m_statusText->setString(status);

				if (finish == true)
				{
					if (m_importer->needDelete())
						m_state = DeleteAsset;
					else
						m_state = Finish;
				}
			}
			else if (m_state == DeleteAsset)
			{
				float percent = 0.0f;
				std::string last;

				bool finish = m_importer->deleteAsset(10);
				m_importer->getDeleteStatus(percent, last);

				m_progressBar->setPercent(percent);

				std::string status = "Deleting...\n";
				status += last;
				m_statusText->setString(status);

				if (finish == true)
					m_state = Finish;
			}

			CSpace::update();
		}

		void CSpaceImport::onDestroy(GUI::CBase* base)
		{
			CSpace::onDestroy(base);
		}

		bool CSpaceImport::isFinish()
		{
			return m_importer->isFinish();
		}
	}
}