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

#pragma once

#include "Utils/CGameSingleton.h"

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
			std::string Guid;
			time_t ModifyTime;
			time_t CreateTime;

			SFileNode(const char* bundle, const char* path, const char* fullPath, const char* guid, time_t modifyTime, time_t createTime)
			{
				Bundle = bundle;
				Path = path;
				FullPath = fullPath;
				Guid = guid;
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

		class CAssetManager : public CGameSingleton<CAssetManager>
		{
		protected:
			std::string m_workingFolder;
			std::string m_assetFolder;

			bool m_haveAssetFolder;

			std::map<std::string, SFileNode*> m_guidToFile;
			std::map<std::string, SFileNode*> m_pathToFile;

			std::list<std::string> m_meta;
			std::list<SFileNode> m_files;
			std::list<SFileNode>::iterator m_fileIterator;
			u32 m_fileID;
			std::string m_lastGUIDFile;

		public:
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

			void update();

			void discoveryAssetFolder();

			void discovery(const std::string& bundle, const std::string& folder);

			bool getFileDate(const char* path, time_t& modifyTime, time_t& createTime);

			std::string generateHash(const char* bundle, const char* path, time_t createTime, time_t now);

			void getRoot(std::vector<SFileInfo>& files);

			void getFolder(const char* folder, std::vector<SFileInfo>& files);

			bool isFolderEmpty(const char* folder);

			void beginLoadGUID();

			bool loadGUID(int count);

			void saveGUID(const char* path, SFileNode& node);

			void readGUID(const char* path, SFileNode& node);

			void removeUnusedMeta();

			bool isLoadGUIDFinish();

			void getLoadGUIDStatus(float& percent, std::string& last);

		protected:

			bool addFileNode(const std::string& bundle, const std::string& path);

		};
	}
}