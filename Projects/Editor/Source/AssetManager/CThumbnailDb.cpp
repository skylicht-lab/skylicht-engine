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

#include "pch.h"
#include "CThumbnailDb.h"

#include <filesystem>

#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		CThumbnailDb::CThumbnailDb()
		{

		}

		CThumbnailDb::~CThumbnailDb()
		{
			clear();
		}

		void CThumbnailDb::clear()
		{
			for (auto it : m_db)
			{
				delete it.second;
			}
			m_db.clear();
		}

		void CThumbnailDb::init()
		{
			if (!fs::exists("../Thumbnails"))
				fs::create_directory("../Thumbnails");
			else
				load();
		}

		void CThumbnailDb::load()
		{
			io::IReadFile* file = getIrrlichtDevice()->getFileSystem()->createAndOpenFile("../Thumbnails/db.txt");
			if (file)
			{
				long i = 0;
				long fileSize = file->getSize();
				char* data = new char[fileSize + 1];
				file->read(data, fileSize);
				data[fileSize] = 0;

				std::string line;
				while (i < fileSize)
				{
					if (data[i] == '\n' || data[i] == '\r')
					{
						line.clear();
					}
					else
					{
						line += data[i];
					}
					i++;
				}

				delete[]data;
				file->drop();
			}
		}

		void CThumbnailDb::save()
		{
			io::IWriteFile* file = getIrrlichtDevice()->getFileSystem()->createAndWriteFile("../Thumbnails/db.txt");
			if (file)
			{
				for (auto it : m_db)
				{
					SThumbnailInfo* info = it.second;
					std::string s = info->Id;
					s += ";";
					s += std::to_string(info->ModifyTime);
					s += ";";
					s += info->Path;
					s += "\n";
					file->write(s.c_str(), (u32)s.size());
				}
				file->drop();
			}
		}

		void CThumbnailDb::addInfo(const char* id, const char* path, time_t modify)
		{
			SThumbnailInfo* info = m_db[id];
			if (info == NULL)
			{
				info = new SThumbnailInfo();
				m_db[id] = info;
			}

			info->Id = id;
			info->ModifyTime = modify;
			info->Path = path;
		}
	}
}