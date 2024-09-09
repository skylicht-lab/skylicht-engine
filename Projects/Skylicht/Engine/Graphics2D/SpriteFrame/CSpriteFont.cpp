/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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
#include "CSpriteFont.h"

#include "Utils/CStringImp.h"
#include "TextureManager/CTextureManager.h"
#include "Graphics2D/Atlas/CAtlas.h"

namespace Skylicht
{
	CSpriteFont::CSpriteFont() :
		m_moduleMap(NULL),
		m_numModuleMap(0)
	{
		m_deleteAtlas = true;
	}

	CSpriteFont::~CSpriteFont()
	{
		if (m_moduleMap != NULL)
		{
			delete m_moduleMap;
			m_moduleMap = NULL;
			m_numModuleMap = 0;
		}
	}

	bool CSpriteFont::loadFont(const char* fileName)
	{
		io::IXMLReader* xmlReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(fileName);
		if (xmlReader == NULL)
			return false;

		// see the font format here
		// https://www.angelcode.com/products/bmfont/doc/file_format.html

		char text[512];

		int readState = -1;

		int moduleID = 0;

		while (xmlReader->read())
		{
			switch (xmlReader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"page")
				{
					m_images.push_back(new SImage());
					SImage* img = m_images.back();

					const wchar_t* id = xmlReader->getAttributeValue(L"id");
					CStringImp::convertUnicodeToUTF8(id, text);
					img->ID = atoi(text);

					const wchar_t* file = xmlReader->getAttributeValue(L"file");
					CStringImp::convertUnicodeToUTF8(file, text);

					char folderPath[128];
					CStringImp::getFolderPath(folderPath, fileName);
					img->Path = folderPath;
					img->Path += "/";
					img->Path += text;

					img->Texture = CTextureManager::getInstance()->getTexture(img->Path.c_str());
				}
				else if (nodeName == L"chars")
				{
					const wchar_t* count = xmlReader->getAttributeValue(L"count");
					CStringImp::convertUnicodeToUTF8(count, text);

					m_numModuleMap = atoi(text);
					m_moduleMap = new int[m_numModuleMap];
				}
				else if (nodeName == L"char")
				{
					// add module
					m_modules.push_back(new SModuleRect());
					SModuleRect* module = m_modules.back();

					// add frame
					m_frames.push_back(new SFrame());
					SFrame* frame = m_frames.back();

					module->ID = moduleID;
					frame->ID = std::to_string(moduleID);

					// frame is a character
					const wchar_t* id = xmlReader->getAttributeValue(L"id");
					CStringImp::convertUnicodeToUTF8(id, text);
					int charID = atoi(text);
					frame->Name = text;

					// read module rect
					const wchar_t* x = xmlReader->getAttributeValue(L"x");
					CStringImp::convertUnicodeToUTF8(x, text);
					module->X = core::fast_atof(text);

					const wchar_t* y = xmlReader->getAttributeValue(L"y");
					CStringImp::convertUnicodeToUTF8(y, text);
					module->Y = core::fast_atof(text);

					const wchar_t* w = xmlReader->getAttributeValue(L"width");
					CStringImp::convertUnicodeToUTF8(w, text);
					module->W = core::fast_atof(text);

					const wchar_t* h = xmlReader->getAttributeValue(L"height");
					CStringImp::convertUnicodeToUTF8(h, text);
					module->H = core::fast_atof(text);

					// read img
					const wchar_t* page = xmlReader->getAttributeValue(L"page");
					CStringImp::convertUnicodeToUTF8(page, text);
					int imageID = atoi(text);
					frame->Image = m_images[imageID];

					// module offset
					frame->ModuleOffset.push_back(SModuleOffset());
					SModuleOffset& moduleOffset = frame->ModuleOffset.back();

					// xoffset
					const wchar_t* xoffset = xmlReader->getAttributeValue(L"xoffset");
					CStringImp::convertUnicodeToUTF8(xoffset, text);
					moduleOffset.OffsetX = core::fast_atof(text);

					// yoffset
					const wchar_t* yoffset = xmlReader->getAttributeValue(L"yoffset");
					CStringImp::convertUnicodeToUTF8(yoffset, text);
					moduleOffset.OffsetY = core::fast_atof(text);

					// xadvance
					const wchar_t* xadvance = xmlReader->getAttributeValue(L"xadvance");
					CStringImp::convertUnicodeToUTF8(xadvance, text);
					moduleOffset.XAdvance = core::fast_atof(text);

					// bounding rect
					frame->BoudingRect.UpperLeftCorner.set(moduleOffset.OffsetX, moduleOffset.OffsetY);
					frame->BoudingRect.LowerRightCorner.set(moduleOffset.OffsetX + module->W, moduleOffset.OffsetY + module->H);

					// map frame to module
					moduleOffset.Frame = frame;
					moduleOffset.Module = module;

					// make module map
					m_moduleMap[moduleID] = charID;
					moduleID++;
				}
			}
			break;
			case io::EXN_TEXT:
			{
				readState = -1;
			}
			break;
			default:
				break;
			}
		};

		xmlReader->drop();

		for (SFrame* frame : m_frames)
			m_names[frame->Name] = frame;

		return true;
	}

	SModuleOffset* CSpriteFont::getCharacterModule(int character)
	{
		if (m_frames.size() == 0)
			return NULL;

		if (m_numModuleMap == 0 || m_moduleMap == NULL)
			return NULL;

		if (character == 128)
			character = 0x20ac;

		const int* p = m_moduleMap;
		const int* q = m_moduleMap + m_numModuleMap;
		const int* r;

		// perform a binary search
		while (p < q - 1)
		{
			r = p + (q - p) / 2;

			if (*r <= character)
				p = r;
			else
				q = r;
		}

		if (character != *p)
			return NULL;


		u32 index = (u32)(p - m_moduleMap);
		if ((u32)m_frames.size() < index)
			return NULL;

		SFrame* frame = m_frames[index];
		return &frame->ModuleOffset[0];
	}
}