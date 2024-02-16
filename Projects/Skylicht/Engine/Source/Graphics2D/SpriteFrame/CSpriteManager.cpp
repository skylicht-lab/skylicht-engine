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
#include "CSpriteManager.h"
#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CSpriteManager::CSpriteManager() :
		m_currentCategory("global")
	{

	}

	CSpriteManager::~CSpriteManager()
	{
		std::vector<CSpriteFrame*> sprites;
		std::vector<SSpritePackage*>::iterator i = m_spriteList.begin(), end = m_spriteList.end();
		while (i != end)
		{
			sprites.push_back((*i)->Sprite);
			i++;
		}

		for (CSpriteFrame* s : sprites)
			releaseSprite(s);
	}

	CSpriteFrame* CSpriteManager::loadSprite(const char* path, const char* category)
	{
		char ansiPath[512];

		if (category == NULL)
		{
			category = m_currentCategory.c_str();
		}

		std::string fixPath = CPath::normalizePath(path);
		strcpy(ansiPath, fixPath.c_str());

		if (CStringImp::length(ansiPath) == 0)
			return NULL;

		if (m_pathToSprite.find(ansiPath) != m_pathToSprite.end())
		{
			return m_pathToSprite[ansiPath];
		}

		CSpriteFrame* sprite = new CSpriteFrame();
		if (sprite->load(path))
		{
			std::string spriteId = sprite->getId();

			m_idToSprite[spriteId] = sprite;
			m_pathToSprite[path] = sprite;

			SSpritePackage* p = new SSpritePackage();
			p->Package = category;
			p->Sprite = sprite;

			m_spriteList.push_back(p);
		}
		else
		{
			delete sprite;
			sprite = NULL;
		}

		return sprite;
	}

	CSpriteFrame* CSpriteManager::getSpriteById(const char* id)
	{
		std::map<std::string, CSpriteFrame*>::iterator it = m_idToSprite.find(id);
		if (it != m_idToSprite.end())
		{
			return it->second;
		}
		return NULL;
	}

	void CSpriteManager::releaseSprite(const char* category)
	{
		std::vector<CSpriteFrame*> sprites;
		std::vector<SSpritePackage*>::iterator i = m_spriteList.begin(), end = m_spriteList.end();
		while (i != end)
		{
			if ((*i)->Package == category)
			{
				sprites.push_back((*i)->Sprite);
			}
			i++;
		}

		for (CSpriteFrame* s : sprites)
			releaseSprite(s);
	}

	void CSpriteManager::releaseSprite(CSpriteFrame* sprite)
	{
		std::map<std::string, CSpriteFrame*>::iterator it = m_idToSprite.find(sprite->getId());
		if (it != m_idToSprite.end())
			m_idToSprite.erase(it);

		it = m_pathToSprite.find(sprite->getPath());
		if (it != m_pathToSprite.end())
			m_pathToSprite.erase(it);

		std::vector<SSpritePackage*>::iterator i = m_spriteList.begin(), end = m_spriteList.end();
		while (i != end)
		{
			if ((*i)->Sprite == sprite)
			{
				delete (*i);
				m_spriteList.erase(i);
				break;
			}
			++i;
		}

		char log[512];
		sprintf(log, "Remove sprite: %s", sprite->getId());
		os::Printer::log(log);

		delete sprite;
	}
}