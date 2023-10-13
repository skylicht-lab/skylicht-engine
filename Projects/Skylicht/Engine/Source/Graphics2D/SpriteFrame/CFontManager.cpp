/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CFontManager.h"
#include "Utils/CPath.h"
#include "Serializable/CSerializableLoader.h"

namespace Skylicht
{
	CFontManager::CFontManager()
	{

	}

	CFontManager::~CFontManager()
	{
		std::vector<CFontSource*> fonts;
		for (auto i : m_pathToFont)
		{
			fonts.push_back(i.second);
		}

		for (CFontSource* font : fonts)
			releaseFont(font);
	}

	CFontSource* CFontManager::loadFontSource(const char* path)
	{
		std::map<std::string, CFontSource*>::iterator it = m_pathToFont.find(path);
		if (it != m_pathToFont.end())
		{
			return it->second;
		}

		CFontSource* fontSource = new CFontSource();
		if (CSerializableLoader::loadSerializable(path, fontSource))
		{
			fontSource->setPath(path);

			m_pathToFont[path] = fontSource;
			m_idToFont[fontSource->getGUID()] = fontSource;
		}
		else
		{
			delete fontSource;
			fontSource = NULL;
		}
		return fontSource;
	}

	CFontSource* CFontManager::getFontById(const char* id)
	{
		std::map<std::string, CFontSource*>::iterator it = m_idToFont.find(id);
		if (it != m_idToFont.end())
			return it->second;
		return NULL;
	}

	void CFontManager::releaseFont(CFontSource* font)
	{
		std::map<std::string, CFontSource*>::iterator it = m_idToFont.find(font->getGUID());
		if (it != m_idToFont.end())
			m_idToFont.erase(it);

		it = m_pathToFont.find(font->getPath());
		if (it != m_pathToFont.end())
			m_pathToFont.erase(it);

		delete font;
	}
}