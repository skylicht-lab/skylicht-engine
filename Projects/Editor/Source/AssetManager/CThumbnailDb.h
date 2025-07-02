/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

namespace Skylicht
{
	namespace Editor
	{
		class CThumbnailDb
		{
		protected:
			struct SThumbnailInfo
			{
				std::string Id;
				std::string Path;
				time_t ModifyTime;
				bool Exists;

				SThumbnailInfo()
				{
					ModifyTime = 0;
					Exists = false;
				}
			};

			std::map<std::string, SThumbnailInfo*> m_db;
		public:
			CThumbnailDb();

			virtual ~CThumbnailDb();

			void clear();

			void init();

			void load();

			void save();

			bool updateInfo(const char* id, const char* path, time_t modify);

			bool saveThumbnailTexture(const char* id);

			std::string getThumbnailFile(const char* id);
		};
	}
}