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
#include "Version.h"
#include "CAssetManager.h"

#include <filesystem>
#include <chrono>
#include <sstream>
#include <sys/stat.h>

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"
#include "Crypto/sha256.h"

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
			m_haveAssetFolder(false)
		{
			m_workingFolder = getIrrlichtDevice()->getFileSystem()->getWorkingDirectory().c_str();

			m_assetFolder = m_workingFolder;
			m_assetFolder = CPath::normalizePath(m_assetFolder);

			m_projectFolder = m_workingFolder + "/..";
			m_projectFolder = CPath::normalizePath(m_projectFolder);

			m_haveAssetFolder = fs::exists(m_assetFolder);
			if (!m_haveAssetFolder)
				os::Printer::log("[CAssetManager] Asset folder is not exists");

		}

		CAssetManager::~CAssetManager()
		{
			for (SFileNode* file : m_files)
			{
				delete file;
			}
			m_files.clear();
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
					{
						addFileNode(bundle, path);
					}
					else
					{
						m_meta.push_back(path);
					}
				}
			}
		}

		void CAssetManager::update()
		{

		}

		SFileNode* CAssetManager::addFileNode(const std::string& bundle, const std::string& path)
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
					new SFileNode(
						bundle.c_str(),
						sortPath.c_str(),
						path.c_str(),
						"",
						modifyTime,
						createTime)
				);

				// map guid
				SFileNode* file = m_files.back();
				m_pathToFile[sortPath] = file;

				return file;
			}

			return NULL;
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
					file.Path = getShortPath(path.c_str());
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
					file.Path = getShortPath(path.c_str());
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

		bool CAssetManager::isFolder(const char* folder)
		{
			return fs::is_directory(folder);
		}

		bool CAssetManager::isExist(const char* path)
		{
			return fs::exists(path);
		}

		void CAssetManager::search(const char* search, std::vector<SFileInfo>& files)
		{
			files.clear();

			wchar_t name[512];

			for (SFileNode* f : m_files)
			{
				if (CPath::searchMatch(f->Path, std::string(search)) == true)
				{
					files.push_back(SFileInfo());
					SFileInfo& file = files.back();

					file.Name = CPath::getFileName(f->Path);
					file.FullPath = f->FullPath;
					file.Path = f->Path;
					file.IsFolder = isFolder(f->FullPath.c_str());
					if (file.IsFolder)
						file.Type = Folder;
					file.Node = m_pathToFile[file.Path];

					CStringImp::convertUTF8ToUnicode(file.Name.c_str(), name);
					file.NameW = name;
				}
			}

			sortFiles(files);
		}

		std::string CAssetManager::getShortPath(const char* folder)
		{
			std::string assetPath = m_assetFolder + "/";
			std::string sortPath = folder;

			if (CStringImp::length(folder) < assetPath.size())
				return std::string("");

			sortPath.replace(sortPath.find(assetPath.c_str()), assetPath.size(), "");
			return sortPath;
		}

		void CAssetManager::deleteChildAsset(const char* folderPath)
		{
			std::string shortPath = getShortPath(folderPath);

			std::list<SFileNode*> deleteList;

			for (SFileNode* node : m_files)
			{
				if (node->Path.find(shortPath) == 0)
				{
					m_pathToFile.erase(node->Path);
					m_guidToFile.erase(node->Guid);

					std::string path = node->FullPath;
					std::string meta = path + ".meta";

					if (fs::exists(meta))
						fs::remove_all(meta);

					// try fix double node in list
					deleteList.remove(node);
					deleteList.push_back(node);
				}
			}

			for (SFileNode* node : deleteList)
			{
				m_files.remove(node);
				delete node;
			}
		}

		bool CAssetManager::deleteAsset(const char* path)
		{
			std::string shortPath = getShortPath(path);

			std::map<std::string, SFileNode*>::iterator it = m_pathToFile.find(shortPath);
			if (it != m_pathToFile.end())
			{
				SFileNode* node = it->second;

				m_pathToFile.erase(node->Path);
				m_guidToFile.erase(node->Guid);
				m_files.remove(node);

				std::string path = node->FullPath;
				std::string meta = path + ".meta";

				fs::remove_all(path);
				fs::remove_all(meta);

				delete node;

				return true;
			}

			return false;
		}

		bool CAssetManager::renameAsset(const char* path, const char* name)
		{
			std::string shortPath = getShortPath(path);
			SFileNode* node = getFileNodeByPath(shortPath.c_str());
			if (node == NULL)
				return false;

			std::string newPath = CPath::getFolderPath(path);
			newPath += "/";
			newPath += name;

			fs::rename(path, newPath.c_str());

			std::string meta = std::string(path) + ".meta";
			std::string newMeta = newPath + ".meta";
			fs::rename(meta, newMeta);

			node->FullPath = newPath;
			node->Path = getShortPath(newPath.c_str());

			m_pathToFile.erase(shortPath);
			m_pathToFile[node->Path] = node;

			if (shortPath == node->Bundle)
			{
				for (SFileNode* f : m_files)
				{
					if (f->Bundle == shortPath)
					{
						f->Bundle = node->Path;
					}
				}
			}

			return true;
		}

		std::string CAssetManager::getBundleName(const char* path)
		{
			std::string shortPath = getShortPath(path);

			std::vector<std::string> splits;
			CStringImp::splitString(shortPath.c_str(), "/", splits);

			std::string bundle = ".";
			if (splits.size() > 1)
				bundle = splits[0];
			return bundle;
		}

		bool CAssetManager::newFolderAsset(const char* path)
		{
			if (fs::create_directory(path))
			{
				std::string bundle = getBundleName(path);

				SFileNode* fileNode = addFileNode(bundle, path);
				m_files.push_back(fileNode);

				readOrGenerateMeta(path, fileNode);
				return true;
			}

			return false;
		}

		SFileNode* CAssetManager::getFileNodeByPath(const char* path)
		{
			std::map<std::string, SFileNode*>::iterator i = m_pathToFile.find(path);
			if (i == m_pathToFile.end())
				return NULL;
			return i->second;
		}

		SFileNode* CAssetManager::getFileNodeByGUID(const char* GUID)
		{
			std::map<std::string, SFileNode*>::iterator i = m_guidToFile.find(GUID);
			if (i == m_guidToFile.end())
				return NULL;
			return i->second;
		}

		bool CAssetManager::readGUID(const char* path, SFileNode* node)
		{
			io::IFileSystem* filesystem = getIrrlichtDevice()->getFileSystem();
			io::IXMLReader* xmlRead = filesystem->createXMLReader(path);
			if (xmlRead == NULL)
				return false;

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
							node->Guid = text;

							xmlRead->drop();
							return true;
						}
					}
					break;
				}
				default:
					break;
				}
			}

			xmlRead->drop();
			return false;
		}

		void CAssetManager::saveGUID(const char* path, SFileNode* node)
		{
			io::IFileSystem* filesystem = getIrrlichtDevice()->getFileSystem();
			io::IWriteFile* file = filesystem->createAndWriteFile(path);
			if (file == NULL)
				return;

			std::string data;
			data += "<meta>\n";
			data += "    <guid id=\"" + node->Guid += "\"/>\n";
			data += "</meta>";

			file->write(data.c_str(), (u32)data.size());
			file->drop();
		}

		void CAssetManager::readOrGenerateMeta(const char* path, SFileNode* node)
		{
			bool regenerate = true;

			std::string meta = std::string(path) + ".meta";

			if (fs::exists(meta))
			{
				// load meta
				if (readGUID(meta.c_str(), node) == true)
				{
					// remove meta
					m_meta.remove(meta);

					// check collision
					if (node->Guid.empty()
						|| node->Guid.size() != 64
						|| m_guidToFile.find(node->Guid) != m_guidToFile.end())
					{
						if (m_guidToFile[node->Guid]->Path != getShortPath(path))
						{
							regenerate = true;

							char log[1024];
							sprintf(log, "[CAssetImporter::loadGUID] GUID Collision: %s\n", node->Path.c_str());
							os::Printer::log(log);
						}
						else
						{
							// file updated, so no need generate guid
							m_guidToFile[node->Guid] = node;

							char log[1024];
							sprintf(log, "[CAssetImporter::loadGUID] GUID Updated: %s\n", node->Path.c_str());
							os::Printer::log(log);
						}
					}
					else
					{
						regenerate = false;

						// map guid
						m_guidToFile[node->Guid] = node;
					}
				}
				else
				{
					regenerate = true;
				}
			}
			else
			{
				regenerate = true;
			}

			if (regenerate)
			{
				// current time
				std::time_t now = std::time(0);

				// generate guid
				node->Guid = generateHash(node->Bundle.c_str(), node->Path.c_str(), node->CreateTime, now);

				// save meta
				saveGUID(meta.c_str(), node);

				// map guid
				m_guidToFile[node->Guid] = node;
			}
		}

		void CAssetManager::registerFileLoader(const char* ext, IFileLoader* loader)
		{
			m_fileLoader[ext] = loader;
		}

		void CAssetManager::unRegisterFileLoader(const char* ext, IFileLoader* loader)
		{
			if (m_fileLoader.find(ext) == m_fileLoader.end())
				return;

			if (m_fileLoader[ext] == loader)
				m_fileLoader[ext] = NULL;
		}

		IFileLoader* CAssetManager::getFileLoader(const char* ext)
		{
			if (m_fileLoader.find(ext) == m_fileLoader.end())
				return NULL;

			return m_fileLoader[ext];
		}
	}
}