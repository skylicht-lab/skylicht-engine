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
#include "CAssetPropertyController.h"
#include "Activator/CEditorActivator.h"
#include "Reactive/CObserver.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	namespace Editor
	{
		CAssetPropertyController::CAssetPropertyController() :
			m_spaceProperty(NULL)
		{

		}

		CAssetPropertyController::~CAssetPropertyController()
		{

		}

		void CAssetPropertyController::onSelectAsset(const char* path, bool isFolder)
		{
			if (m_spaceProperty == NULL)
				return;

			// Name and icon
			if (isFolder)
				m_spaceProperty->setIcon(GUI::ESystemIcon::Folder);
			else
				m_spaceProperty->setIcon(GUI::ESystemIcon::File);

			std::string fileName = CPath::getFileName(path);
			std::string ext = CPath::getFileNameExt(path);

			m_spaceProperty->setLabel(CStringImp::convertUTF8ToUnicode(fileName.c_str()).c_str());

			// Clear old ui
			m_spaceProperty->clearAllGroup();

			// Tabable
			m_spaceProperty->getWindow()->getCanvas()->TabableGroup.clear();

			// Get file ext editor
			if (!isFolder)
			{
				CAssetEditor* assetEditor = CEditorActivator::getInstance()->getAssetEditorInstance(ext.c_str());
				if (assetEditor != NULL)
					m_spaceProperty->addAsset(assetEditor, path);
			}
		}
	}
}