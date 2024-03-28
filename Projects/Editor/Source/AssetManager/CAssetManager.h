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

#pragma once

#include "Utils/CSingleton.h"

#include <functional>

namespace Skylicht
{
	namespace Editor
	{
		struct SFileNode
		{
			std::string Bundle;
			std::string Path;
			std::string FullPath;
			time_t ModifyTime;
			time_t CreateTime;

			SFileNode(const char* bundle, const char* path, const char* fullPath, time_t modifyTime, time_t createTime)
			{
				Bundle = bundle;
				Path = path;
				FullPath = fullPath;
				ModifyTime = modifyTime;
				CreateTime = createTime;
			}
		};

		enum EFileType
		{
			Unknown = 0,
			Folder,
			Model3D,
			Mesh,
			Texture,
			Script
		};

		struct SFileInfo
		{
			std::string Name;
			std::wstring NameW;
			std::string Path;
			std::string FullPath;
			bool IsFolder;
			EFileType Type;
			SFileNode* Node;

			SFileInfo()
			{
				IsFolder = false;
				Type = Unknown;
				Node = NULL;
			}
		};

		class IFileLoader
		{
		public:
			virtual void loadFile(const std::string& path) = 0;
		};

		class CAssetImporter;
		class CAssetWatcher;

		class CAssetManager
		{
		public:
			DECLARE_SINGLETON(CAssetManager)

		protected:
			std::string m_workingFolder;
			std::string m_assetFolder;
			std::string m_projectFolder;

			bool m_haveAssetFolder;

			std::map<std::string, SFileNode*> m_pathToFile;

			std::list<SFileNode*> m_files;

			std::map<std::string, IFileLoader*> m_fileLoader;

		public:

			friend class CAssetImporter;
			friend class CAssetWatcher;

			CAssetManager();

			virtual ~CAssetManager();

			inline const std::string& getAssetFolder()
			{
				return m_assetFolder;
			}

			inline const std::string& getWorkingFolder()
			{
				return m_workingFolder;
			}

			inline std::list<SFileNode*>& getListFiles()
			{
				return m_files;
			}

			void update();

			void discoveryAssetFolder();

			bool getFileDate(const char* path, time_t& modifyTime, time_t& createTime);

			void getRoot(std::vector<SFileInfo>& files);

			void getFolder(const char* folder, std::vector<SFileInfo>& files);

			bool isFolderEmpty(const char* folder);

			bool isFolder(const char* folder);

			bool isExist(const char* path);

			void search(const char* search, std::vector<SFileInfo>& files);

			std::string getShortPath(const char* folder);

			std::string generateAssetPath(const char* pattern, const char* currentFolder);

			SFileNode* getFileNodeByPath(const char* path);

		public:

			void deleteChildAsset(const char* folderPath);

			bool deleteAsset(const char* path);

			bool renameAsset(const char* path, const char* name);

			bool newFolderAsset(const char* path);

			std::string getBundleName(const char* path);

			std::string getGenerateMetaGUID(const char* path);

		public:

			void registerFileLoader(const char* ext, IFileLoader* loader);

			void unRegisterFileLoader(const char* ext, IFileLoader* loader);

			IFileLoader* getFileLoader(const char* ext);

		protected:

			void discovery(const std::string& bundle, const std::string& folder);

			SFileNode* addFileNode(const std::string& bundle, const std::string& path);

			void sortFiles(std::vector<SFileInfo>& files);
		};
	}
}