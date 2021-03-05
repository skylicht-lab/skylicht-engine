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
#include "Version.h"
#include "CAssetManager.h"

#include <filesystem>
#include <chrono>
#include <sstream>
#include <sys/stat.h>

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"
#include "Crypto/sha256.h"

#include "FileWatcher/FileWatcher.h"

#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		CAssetManager::CAssetManager() :
			m_haveAssetFolder(false),
			m_fileID(0)
		{
			m_workingFolder = getIrrlichtDevice()->getFileSystem()->getWorkingDirectory().c_str();

			m_assetFolder = m_workingFolder + "/../Assets";
			m_assetFolder = CPath::normalizePath(m_assetFolder);

			m_haveAssetFolder = fs::exists(m_assetFolder);
			if (!m_haveAssetFolder)
				os::Printer::log("[CAssetManager] Asset folder is not exists");

		}

		CAssetManager::~CAssetManager()
		{

		}

		void CAssetManager::discoveryAssetFolder()
		{
			m_files.clear();
			m_guidToFile.clear();
			m_pathToFile.clear();
			m_meta.clear();

			if (m_haveAssetFolder)
			{
				for (const auto& file : fs::directory_iterator(m_assetFolder))
				{
					std::string path = file.path().generic_u8string();

					if (file.is_directory())
					{
						std::string bundle = CPath::getFileName(path);
						discovery(bundle.c_str(), path.c_str());
					}
					else
					{
						if (CPath::getFileNameExt(path) != "meta")
							addFileNode(".", path);
						else
							m_meta.push_back(path);
					}
				}
			}
		}

		void CAssetManager::discovery(const std::string& bundle, const std::string& folder)
		{
			addFileNode(bundle, folder);

			for (const auto& file : fs::directory_iterator(folder))
			{
				std::string path = file.path().generic_u8string();

				if (file.is_directory())
					discovery(bundle, path);
				else
				{
					if (CPath::getFileNameExt(path) != "meta")
						addFileNode(bundle, path);
					else
						m_meta.push_back(path);
				}
			}
		}

		void CAssetManager::update()
		{

		}

		bool CAssetManager::addFileNode(const std::string& bundle, const std::string& path)
		{
			std::string assetPath = m_assetFolder + "/";
			time_t modifyTime, createTime;

			if (getFileDate(path.c_str(), modifyTime, createTime) == true)
			{
				// get short path
				std::string sortPath = path;
				sortPath.replace(sortPath.find(assetPath.c_str()), assetPath.size(), "");

				// add db
				m_files.push_back(
					SFileNode(
						bundle.c_str(),
						sortPath.c_str(),
						path.c_str(),
						"",
						modifyTime,
						createTime)
				);

				// map guid
				SFileNode& file = m_files.back();
				m_pathToFile[sortPath] = &file;

				return true;
			}

			return false;
		}

		bool CAssetManager::getFileDate(const char* path, time_t& modifyTime, time_t& createTime)
		{
			struct stat result;
			if (stat(path, &result) == 0)
			{
				modifyTime = result.st_mtime;
				createTime = result.st_ctime;
				return true;
			}

			return false;
		}

		std::string CAssetManager::generateHash(const char* bundle, const char* path, time_t createTime, time_t now)
		{
			std::string hashString = std::string(bundle) + std::string(":") + path;
			hashString += ":";
			hashString += std::to_string(createTime);
			hashString += ":";
			hashString += std::to_string(now);

			BYTE8 buf[SHA256_BLOCK_SIZE];
			SHA256_CTX ctx;

			sha256_init(&ctx);
			sha256_update(&ctx, (const BYTE8*)hashString.c_str(), hashString.length());
			sha256_final(&ctx, buf);

			std::stringstream result;
			for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
				result << std::setfill('0') << std::setw(2) << std::hex << (int)buf[i];

			return result.str();
		}

		void CAssetManager::getRoot(std::vector<SFileInfo>& files)
		{
			getFolder(m_assetFolder.c_str(), files);
		}

		void CAssetManager::getFolder(const char* folder, std::vector<SFileInfo>& files)
		{
			files.clear();
			std::string assetPath = m_assetFolder + "/";
			wchar_t name[512];

			for (const auto& file : fs::directory_iterator(folder))
			{
				std::string path = file.path().generic_u8string();

				if (file.is_directory())
				{
					files.push_back(SFileInfo());
					SFileInfo& file = files.back();

					file.Name = CPath::getFileName(path);
					file.FullPath = path;
					file.Path = path;
					file.Path.replace(file.Path.find(assetPath.c_str()), assetPath.size(), "");
					file.IsFolder = true;
					file.Type = Folder;
					file.Node = m_pathToFile[file.Path];

					CStringImp::convertUTF8ToUnicode(file.Name.c_str(), name);
					file.NameW = name;
				}
				else
				{
					if (CPath::getFileNameExt(path) == "meta")
						continue;

					files.push_back(SFileInfo());
					SFileInfo& file = files.back();

					file.Name = CPath::getFileName(path);
					file.FullPath = path;
					file.Path = path;
					file.Path.replace(file.Path.find(assetPath.c_str()), assetPath.size(), "");
					file.IsFolder = false;
					file.Node = m_pathToFile[file.Path];

					CStringImp::convertUTF8ToUnicode(file.Name.c_str(), name);
					file.NameW = name;

					std::string ext = CPath::getFileNameExt(path);
				}
			}

			sortFiles(files);
		}

		void CAssetManager::sortFiles(std::vector<SFileInfo>& files)
		{
			std::sort(files.begin(), files.end(),
				[](SFileInfo& a, SFileInfo& b) {

					if (a.IsFolder && b.IsFolder)
					{
						return a.Name < b.Name;
					}
					else if (!a.IsFolder && !b.IsFolder)
					{
						return a.Name < b.Name;
					}
					else
					{
						if (b.IsFolder == false)
							return true;

						return false;
					}

					return false;
				}
			);
		}

		bool CAssetManager::isFolderEmpty(const char* folder)
		{
			for (const auto& file : fs::directory_iterator(folder))
			{
				std::string path = file.path().generic_u8string();

				if (file.is_directory())
				{
					return false;
				}
			}

			return true;
		}

		void CAssetManager::beginLoadGUID()
		{
			m_fileIterator = m_files.begin();
			m_fileID = 0;
		}

		bool CAssetManager::loadGUID(int count)
		{
			if (m_fileIterator == m_files.end())
				return true;

			std::time_t now = std::time(0);

			for (int j = 0; j < count; j++)
			{
				SFileNode& node = (*m_fileIterator);

				std::string path = node.FullPath;
				std::string meta = path + ".meta";

				bool regenerate = true;

				if (fs::exists(meta))
				{
					// load meta
					readGUID(meta.c_str(), node);

					// remove meta
					m_meta.remove(meta);

					// check collision
					if (node.Guid.empty() || node.Guid.size() != 64 || m_guidToFile.find(node.Guid) != m_guidToFile.end())
					{
						regenerate = true;

						char log[1024];
						sprintf(log, "[CAssetManager::loadGUID] GUID Collision: %s\n", node.Path.c_str());
						os::Printer::log(log);
					}
					else
					{
						regenerate = false;

						// map guid
						m_guidToFile[node.Guid] = &node;
					}
				}
				else
				{
					regenerate = true;
				}

				if (regenerate)
				{
					// generate guid
					node.Guid = generateHash(node.Bundle.c_str(), node.Path.c_str(), node.CreateTime, now);

					// save meta
					saveGUID(meta.c_str(), node);

					// map guid
					m_guidToFile[node.Guid] = &node;
				}

				m_lastGUIDFile = node.Path;

				++m_fileIterator;
				++m_fileID;

				if (m_fileIterator == m_files.end())
				{
					removeUnusedMeta();
					return true;
				}
			}

			return false;
		}

		void CAssetManager::readGUID(const char* path, SFileNode& node)
		{
			io::IFileSystem* filesystem = getIrrlichtDevice()->getFileSystem();
			io::IXMLReader* xmlRead = filesystem->createXMLReader(path);
			if (xmlRead == NULL)
				return;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					std::wstring nodeName = xmlRead->getNodeName();
					if (nodeName == L"guid")
					{
						const wchar_t* value = xmlRead->getAttributeValue(L"id");
						if (value != NULL)
						{
							char text[70];
							CStringImp::convertUnicodeToUTF8(value, text);
							node.Guid = text;

							xmlRead->drop();
							return;
						}
					}
					break;
				}
				default:
					break;
				}
			}

			xmlRead->drop();
		}

		void CAssetManager::saveGUID(const char* path, SFileNode& node)
		{
			io::IFileSystem* filesystem = getIrrlichtDevice()->getFileSystem();
			io::IWriteFile* file = filesystem->createAndWriteFile(path);
			if (file == NULL)
				return;

			std::string data;
			data += "<meta>\n";
			data += "\t<guid id=\"" + node.Guid += "\"/>\n";
			data += "</meta>";

			file->write(data.c_str(), data.size());
			file->drop();
		}

		void CAssetManager::getLoadGUIDStatus(float& percent, std::string& last)
		{
			percent = m_fileID / (float)(m_files.size());
			last = m_lastGUIDFile;
		}

		bool CAssetManager::isLoadGUIDFinish()
		{
			if (m_fileIterator == m_files.end())
				return true;

			return false;
		}

		void CAssetManager::removeUnusedMeta()
		{
			for (const std::string& path : m_meta)
			{
				fs::remove(path);
			}

			m_meta.clear();
		}
	}
}