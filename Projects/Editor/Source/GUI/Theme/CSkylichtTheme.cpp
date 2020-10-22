/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CSkylichtTheme.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtTheme::CSkylichtTheme()
			{
				m_sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 512, 512);

				m_empty = m_sprite->addFrame("empty", "Editor/GUI/empty.png");
				m_window = m_sprite->addFrame("draw_window", "Editor/GUI/draw_window.png");
				m_windowShadow = m_sprite->addFrame("draw_window_shadow", "Editor/GUI/draw_window_shadow.png");

				m_sprite->updateTexture();

				m_graphics = CGraphics2D::getInstance();
				m_renderer = dynamic_cast<CSkylichtRenderer*>(CRenderer::getRenderer());
				m_materialID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
			}

			CSkylichtTheme::~CSkylichtTheme()
			{
				delete m_sprite;
			}

			core::rectf CSkylichtTheme::getRect(const SRect& rect)
			{
				return core::rectf(rect.X, rect.Y, rect.X + rect.Width, rect.Y + rect.Height);
			}

			void CSkylichtTheme::drawWindow(const SRect& rect, bool isFocussed)
			{
				SColor color(252, 35, 35, 35);

				m_graphics->addModuleBatch(
					&m_window->ModuleOffset[0],
					color,
					m_renderer->getWorldTransform(),
					getRect(rect),
					11.0f, 55.0f, 11.0f, 55.0f,
					m_materialID);
			}

			void CSkylichtTheme::drawWindowShadow(const SRect& rect)
			{
				SColor color(255, 255, 255, 255);

				m_graphics->addModuleBatch(
					&m_windowShadow->ModuleOffset[0],
					color,
					m_renderer->getWorldTransform(),
					getRect(rect),
					11.0f, 55.0f, 11.0f, 55.0f,
					m_materialID);
			}
		}
	}
}