/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "GUI/CGUIContext.h"
#include "CSpaceSprite.h"
#include "AssetManager/CAssetManager.h"
#include "Editor/CEditor.h"

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
		CSpaceSprite::CSpaceSprite(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_sprite(NULL)
		{
			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			m_label = new GUI::CLabel(toolbar);
			m_label->setPadding(GUI::SPadding(10.0f, 2.0f, 0.0f, 0.0f));
			m_label->setString(L"");
			m_label->sizeToContents();
			toolbar->addControl(m_label);

			m_view = new GUI::CThumbnailView(window, 256.0f);
			m_view->dock(GUI::EPosition::Fill);
		}

		CSpaceSprite::~CSpaceSprite()
		{
			if (m_sprite)
				delete m_sprite;
		}

		void CSpaceSprite::openSprite(const std::string& path)
		{
			if (m_sprite)
				delete m_sprite;

			m_sprite = new CSpriteFrame();
			m_sprite->load(path.c_str());

			std::string shortPath = CAssetManager::getInstance()->getShortPath(path.c_str());
			m_label->setString(CStringImp::convertUTF8ToUnicode(shortPath.c_str()));
			m_label->sizeToContents();
		}

		void CSpaceSprite::update()
		{
			CSpace::update();
		}

		void CSpaceSprite::onDestroy(GUI::CBase* base)
		{
			CSpace::onDestroy(base);
		}
	}
}