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
#include "Utils/CPath.h"

namespace Skylicht
{
	namespace Editor
	{
		CAssetWatcher::CAssetWatcher()
		{
			m_assetManager = CAssetManager::getInstance();
			m_fileWatcher = new FW::FileWatcher();
		}

		CAssetWatcher::~CAssetWatcher()
		{
			delete m_fileWatcher;
		}

		void CAssetWatcher::update()
		{
			m_fileWatcher->update();
		}

		void CAssetWatcher::beginWatch()
		{
			std::string assetFolder = m_assetManager->getAssetFolder();

#if defined(WIN32)
			std::wstring assetFolderW = CStringImp::convertUTF8ToUnicode(assetFolder.c_str());
			m_watchID = m_fileWatcher->addWatch(assetFolderW, this, true);
#else
			m_watchID = m_fileWatcher->addWatch(assetFolder, this, true);
#endif
		}

		void CAssetWatcher::endWatch()
		{
			m_fileWatcher->removeWatch(m_watchID);
		}

		void CAssetWatcher::handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action)
		{
			if (m_watchID != watchid)
				return;

			std::string directory;
			std::string file;

#if defined(WIN32)
			directory = CStringImp::convertUnicodeToUTF8(dir.c_str());
			file = CStringImp::convertUnicodeToUTF8(filename.c_str());
#else
			directory = dir;
			file = filename;
#endif

			if (CPath::getFileNameExt(file) == "meta")
				return;

			std::string path = directory + "/" + file;
			path = CStringImp::replaceAll(path, std::string("\\"), std::string("/"));

			// std::string log;

			switch (action)
			{
			case FW::Action::Add:
				// log += "Add: ";
				// log += path;
				// os::Printer::log(log.c_str());
				m_add.push_back(path);
				break;
			case FW::Action::Delete:
			{
				// log += "Delete: ";
				// log += path;
				// os::Printer::log(log.c_str());

				bool addToDelete = true;

				std::list<std::string>::iterator i = m_add.begin(), end = m_add.end();
				while (i != end)
				{
					if (*i == path)
					{
						m_add.erase(i);
						addToDelete = false;
						break;
					}
					++i;
				}

				if (addToDelete)
					m_delete.push_back(path);
			}
			break;
			case FW::Action::Modified:
			{
				// log += "Modified: ";
				// log += path;
				// os::Printer::log(log.c_str());
			}
			break;
			default:
				break;
			}
		}

		void CAssetWatcher::lock()
		{
			for (std::string& path : m_add)
			{
				// read bundle from path
				std::string bundle = m_assetManager->getBundleName(path.c_str());

				// add file or folder
				m_files.push_back(m_assetManager->addFileNode(bundle, path));
			}

			m_add.clear();
		}

		void CAssetWatcher::unlock()
		{
			m_files.clear();
			m_delete.clear();
		}
	}
}