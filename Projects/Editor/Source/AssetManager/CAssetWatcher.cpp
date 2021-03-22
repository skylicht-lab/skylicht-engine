#/*
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
#include "CAssetWatcher.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CAssetWatcher::CAssetWatcher()
		{
			m_assetManager = CAssetManager::getInstance();

			std::string assetFolder = m_assetManager->getAssetFolder();

			m_fileWatcher = new FW::FileWatcher();

#if defined(WIN32)
			std::wstring assetFolderW = CStringImp::convertUTF8ToUnicode(assetFolder.c_str());
			m_fileWatcher->addWatch(assetFolderW, this, true);
#else
			m_fileWatcher->addWatch(assetFolder, this, true);
#endif
		}

		CAssetWatcher::~CAssetWatcher()
		{
			delete m_fileWatcher;
		}

		void CAssetWatcher::update()
		{
			m_fileWatcher->update();
		}

		void CAssetWatcher::handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action)
		{
			std::string directory;
			std::string file;

#if defined(WIN32)
			directory = CStringImp::convertUnicodeToUTF8(dir.c_str());
			file = CStringImp::convertUnicodeToUTF8(filename.c_str());
#else
			directory = dir;
			file = filename;
#endif

			std::string path = directory + "/" + file;

			switch (action)
			{
			case FW::Action::Add:

				break;
			case FW::Action::Delete:

				break;
			case FW::Action::Modified:
				break;
			default:
				break;
			}

		}
	}
}