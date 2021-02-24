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
			std::string Path;
			std::string Guid;
			time_t ModifyTime;
			time_t CreateTime;

			SFileNode(const char* path, const char* guid, time_t modifyTime, time_t createTime)
			{
				Path = path;
				Guid = guid;
				ModifyTime = modifyTime;
				CreateTime = createTime;
			}
		};

		class CAssetManager : public CGameSingleton<CAssetManager>
		{
		protected:
			std::string m_workingFolder;
			std::string m_assetFolder;

			bool m_haveAssetFolder;

			std::list<SFileNode> m_files;

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

			void discovery(const std::string& folder);

			bool getFileDate(const char* path, time_t& modifyTime, time_t& createTime);

			std::string generateHash(const char* path, time_t createTime, time_t now);
		};
	}
}