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
#include "GUI/CGUIContext.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtTheme::CSkylichtTheme()
			{
				m_renderer = dynamic_cast<CSkylichtRenderer*>(CRenderer::getRenderer());
				m_graphics = CGraphics2D::getInstance();
				m_materialID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");

				m_sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 1024, 1024);

				m_empty = m_sprite->addFrame("empty", "Editor/GUI/empty.png");
				m_window = m_sprite->addFrame("draw_window", "Editor/GUI/draw_window.png");
				m_windowShadow = m_sprite->addFrame("draw_window_shadow", "Editor/GUI/draw_window_shadow.png");

				m_renderer->initFont(m_sprite);

				m_sprite->updateTexture();
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
				float left = 11.0f;
				float top = 11.0f;
				float right = 55.0f;
				float bottom = 55.0f;

				SModuleOffset *module = &m_window->ModuleOffset[0];

				core::rectf r = getRect(rect);
				r.UpperLeftCorner.X = r.UpperLeftCorner.X - left;
				r.UpperLeftCorner.Y = r.UpperLeftCorner.Y - top;
				r.LowerRightCorner.X = r.LowerRightCorner.X + (module->Module->W - right);
				r.LowerRightCorner.Y = r.LowerRightCorner.Y + (module->Module->H - bottom);

				m_graphics->addModuleBatch(
					module,
					getColor(CThemeConfig::WindowBackgroundColor),
					m_renderer->getWorldTransform(),
					r,
					left, right, top, bottom,
					m_materialID);
			}

			void CSkylichtTheme::drawWindowShadow(const SRect& rect)
			{
				float left = 11.0f;
				float top = 11.0f;
				float right = 55.0f;
				float bottom = 55.0f;

				SModuleOffset *module = &m_windowShadow->ModuleOffset[0];

				core::rectf r = getRect(rect);
				r.UpperLeftCorner.X = r.UpperLeftCorner.X - left;
				r.UpperLeftCorner.Y = r.UpperLeftCorner.Y - top;
				r.LowerRightCorner.X = r.LowerRightCorner.X + (module->Module->W - right);
				r.LowerRightCorner.Y = r.LowerRightCorner.Y + (module->Module->H - bottom);

				m_graphics->addModuleBatch(
					module,
					getColor(CThemeConfig::White),
					m_renderer->getWorldTransform(),
					r,
					left, right, top, bottom,
					m_materialID);
			}

			video::SColor CSkylichtTheme::getColor(const SGUIColor& color)
			{
				return SColor(color.A, color.R, color.G, color.B);
			}
		}
	}
}