/*
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
#include "CAssetManager.h"

#include <filesystem>
#include <chrono>

#include "Utils/CPath.h"

#include "FileWatcher/FileWatcher.h"

namespace Skylicht
{
	namespace Editor
	{
		CAssetManager::CAssetManager()
		{
			m_workingFolder = getIrrlichtDevice()->getFileSystem()->getWorkingDirectory().c_str();

			m_assetFolder = m_workingFolder + "/../Assets";
			m_assetFolder = CPath::normalizePath(m_assetFolder);

			m_haveAssetFolder = std::filesystem::exists(m_assetFolder);
			if (!m_haveAssetFolder)
				os::Printer::log("[CAssetManager] Asset folder is not exists");

		}

		CAssetManager::~CAssetManager()
		{

		}

		void CAssetManager::discoveryAssetFolder()
		{
			m_files.clear();

			if (m_haveAssetFolder)
				discovery(m_assetFolder);
		}

		void CAssetManager::discovery(const std::string& folder)
		{
			for (const auto& file : std::filesystem::directory_iterator(folder))
			{
				std::string path = file.path().generic_u8string();

				if (file.is_directory())
					discovery(path);
				else
				{
					m_files.push_back(SFileNode(path.c_str(), "", getModifyDate(path.c_str())));
				}
			}
		}

		void CAssetManager::update()
		{

		}

		time_t CAssetManager::getModifyDate(const char* path)
		{
			struct stat result;
			if (stat(path, &result) == 0)
			{
				return result.st_mtime;
			}

			return 0;
		}
	}
}