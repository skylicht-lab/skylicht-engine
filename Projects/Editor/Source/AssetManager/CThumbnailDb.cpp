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
#include "CAssetManager.h"
#include "Utils/CStringImp.h"
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
			clearTextures();
		}

		void CThumbnailDb::clear()
		{
			for (auto it : m_db)
			{
				delete it.second;
			}
			m_db.clear();
		}

		void CThumbnailDb::clearTextures()
		{
			IVideoDriver* driver = getVideoDriver();
			for (auto it : m_textures)
			{
				ITexture* texture = it.second;
				if (texture && texture->getReferenceCount() == 1)
				{
					driver->removeTexture(texture);
					
					m_textures[it.first] = NULL;
				}
			}
			
			bool done = true;
			do {
				done = true;
				for (auto it : m_textures)
				{
					if (it.second == NULL)
					{
						m_textures.erase(it.first);
						done = false;
						break;
					}
				}
			} while (!done);
		}

		ITexture* CThumbnailDb::getThumbnail(const char* path)
		{
			std::string id = CAssetManager::getInstance()->getGenerateMetaGUID(path);
			if (m_textures[id])
				return m_textures[id];

			SThumbnailInfo* info = m_db[id];
			if (info == NULL)
				return NULL;

			std::string thumbnailFile = getThumbnailFile(id.c_str());
			ITexture* texture = getVideoDriver()->getTexture(thumbnailFile.c_str());
			if (texture)
				m_textures[id] = texture;

			return texture;
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
			io::IXMLReader* reader = getIrrlichtDevice()->getFileSystem()->createXMLReader("../Thumbnails/.db.xml");
			if (reader)
			{
				std::wstring nodeName = L"node";

				while (reader->read())
				{
					switch (reader->getNodeType())
					{
					case io::EXN_ELEMENT:
					{
						// <node>
						if (nodeName == reader->getNodeName())
						{
							const wchar_t* guid = reader->getAttributeValue(L"guid");
							const wchar_t* path = reader->getAttributeValue(L"path");
							const wchar_t* modify = reader->getAttributeValue(L"modify");
							if (guid && path && modify)
							{
								SThumbnailInfo* info = new SThumbnailInfo();
								info->Id = CStringImp::convertUnicodeToUTF8(guid);
								info->Path = CStringImp::convertUnicodeToUTF8(path);
								info->ModifyTime = wcstol(modify, nullptr, 10);
								info->Exists = false;

								std::string file = getThumbnailFile(info->Id.c_str());
								if (!fs::exists(file))
									delete info;
								else
									m_db[info->Id] = info;
							}
						}
					}
					break;
					default:
						break;
					}
				}
				reader->drop();
			}
		}

		void CThumbnailDb::save()
		{
			io::IXMLWriter* file = getIrrlichtDevice()->getFileSystem()->createXMLWriter("../Thumbnails/.db.xml");
			if (file)
			{
				core::array<core::stringw> names;
				core::array<core::stringw> attributes;

				names.push_back(L"guid");
				names.push_back(L"path");
				names.push_back(L"modify");

				file->writeXMLHeader();
				file->writeElement(L"db");
				file->writeLineBreak();

				for (auto it : m_db)
				{
					SThumbnailInfo* info = it.second;
					if (info->Exists)
					{
						attributes.clear();
						attributes.push_back(info->Id.c_str());
						attributes.push_back(info->Path.c_str());
						attributes.push_back(std::to_string(info->ModifyTime).c_str());

						file->writeElement(L"node", true, names, attributes);
						file->writeLineBreak();
					}
				}

				file->writeClosingTag(L"db");
				file->drop();
			}
		}

		bool CThumbnailDb::updateInfo(const char* id, const char* path, time_t modify)
		{
			SThumbnailInfo* info = m_db[id];
			if (info == NULL)
			{
				info = new SThumbnailInfo();
				m_db[id] = info;
			}

			bool ret = info->ModifyTime != modify;

			info->Id = id;
			info->ModifyTime = modify;
			info->Path = path;
			info->Exists = true;

			if (ret == false)
			{
				std::string file = getThumbnailFile(info->Id.c_str());
				if (!fs::exists(file))
					ret = true;
			}

			return ret;
		}

		bool CThumbnailDb::saveThumbnailTexture(const char* id)
		{
			SThumbnailInfo* info = m_db[id];
			if (info == NULL)
				return false;

			video::IVideoDriver* driver = getVideoDriver();

			IImage* img = driver->createImageFromFile(info->Path.c_str());
			if (img == NULL)
				return false;

			core::dimension2d s = img->getDimension();

			u32 max = core::max_(s.Width, s.Height);
			float scale = max <= 64 ? 1.0f : 64 / (float)max;

			u32 w = (u32)(s.Width * scale);
			u32 h = (u32)(s.Height * scale);

			IImage* scaleImg = driver->createImage(img->getColorFormat(), core::dimension2du(w, h));
			img->copyToScaling(scaleImg);

			std::string output = getThumbnailFile(info->Id.c_str());
			driver->writeImageToFile(scaleImg, output.c_str());

			scaleImg->drop();
			img->drop();
			return false;
		}

		std::string CThumbnailDb::getThumbnailFile(const char* id)
		{
			std::string file = "../Thumbnails/";
			file += id;
			file += ".png";
			return file;
		}
	}
}
