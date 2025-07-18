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
#include "Utils/CRandomID.h"

#include "Serializable/CAssetResource.h"
#include "Serializable/CSerializableLoader.h"

#include "TextureManager/CTextureManager.h"
#include "MeshManager/CMeshManager.h"

#include "ResourceSettings/CTextureSettings.h"
#include "ResourceSettings/CMeshExportSettings.h"

#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CAssetManager);

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
			else
				m_thumbnail.init();
		}

		CAssetManager::~CAssetManager()
		{
			clearFiles();
		}

		void CAssetManager::clearFiles()
		{
			for (SFileNode* file : m_files)
			{
				delete file;
			}
			m_files.clear();
			m_pathToFile.clear();
		}

		void CAssetManager::discoveryAssetFolder()
		{
			clearFiles();

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
						addFileNode(".", path);
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
					addFileNode(bundle, path);
			}
		}

		void CAssetManager::update()
		{

		}

		SFileNode* CAssetManager::getFileNode(const char* path)
		{
			auto it = m_pathToFile.find(std::string(path));
			if (it != m_pathToFile.end())
				return it->second;
			return NULL;
		}

		SFileNode* CAssetManager::addFileNode(const std::string& bundle, const std::string& path)
		{
			std::string assetPath = m_assetFolder + "/";
			time_t modifyTime, createTime;

			if (getFileDate(path.c_str(), modifyTime, createTime) == true)
			{
				// get short path
				std::string sortPath = path;

				size_t pos = sortPath.find(assetPath.c_str());
				if (pos == 0)
					sortPath.replace(pos, assetPath.size(), "");

				// add db
				m_files.push_back(
					new SFileNode(
						bundle.c_str(),
						sortPath.c_str(),
						path.c_str(),
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
			std::string assetPath = m_assetFolder;
			std::string sortPath = folder;

			if (CStringImp::length(folder) < assetPath.size())
				return std::string(folder);

			if (sortPath.find(assetPath.c_str()) == 0)
				sortPath.replace(0, assetPath.size(), "");

			while (!sortPath.empty() && sortPath[0] == '/')
				sortPath.erase(sortPath.begin());

			return sortPath;
		}

		std::string CAssetManager::generateAssetPath(const char* pattern, const char* currentFolder)
		{
			int i = 1;
			bool exists = false;
			char name[256];

			std::string fullPath;
			do
			{
				sprintf(name, pattern, i);
				fullPath = currentFolder;
				fullPath += name;
				exists = isExist(fullPath.c_str());
				i++;
			} while (exists);

			return fullPath;
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
				if (node)
				{
					m_files.remove(node);

					std::string path = node->FullPath;

					if (fs::is_directory(path))
						fs::remove_all(path);
					else
					{
						fs::remove(path);
						
						std::string meta = path + ".meta";
						if (fs::exists(meta.c_str()))
							fs::remove(meta.c_str());
					}
					
					m_pathToFile.erase(it);

					delete node;
					return true;
				}
				else
				{
					char log[512];
					sprintf(log, "[CAssetManager] Delete %s with null infomation", shortPath.c_str());
					os::Printer::log(log);
					m_pathToFile.erase(it);
					return true;
				}
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
			if (fs::exists(meta.c_str()))
				fs::rename(meta.c_str(), newMeta.c_str());
			
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

		std::string CAssetManager::getGenerateMetaGUID(const char* path)
		{
			std::string meta = path;
			meta += ".meta";

			CAssetResource* asset = NULL;

			std::string ext = CPath::getFileNameExt(path);
			ext = CStringImp::toLower(ext);

			if (CTextureManager::getInstance()->isTextureExt(ext.c_str()))
				asset = new CTextureSettings();
			else if (CMeshManager::getInstance()->isMeshExt(ext.c_str()))
				asset = new CMeshExportSettings();
			else
				asset = new CAssetResource("CAssetResource");

			if (!CSerializableLoader::loadSerializable(meta.c_str(), asset) || asset->GenerateNewId)
			{
				asset->save(meta.c_str());
			}

			std::string result = asset->GUID.get();
			delete asset;

			return result;
		}

		bool CAssetManager::newFolderAsset(const char* path)
		{
			if (fs::create_directory(path))
			{
				std::string bundle = getBundleName(path);

				SFileNode* fileNode = addFileNode(bundle, path);
				m_files.push_back(fileNode);

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
