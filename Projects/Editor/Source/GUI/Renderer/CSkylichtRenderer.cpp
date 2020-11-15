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
#include "CSkylichtRenderer.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Theme/CSkylichtTheme.h"
#include "Graphics2D/Glyph/CGlyphFreetype.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtRenderer::CSkylichtRenderer(float w, float h) :
				m_width(w),
				m_height(h),
				m_fontLarge(NULL),
				m_fontNormal(NULL)
			{
				m_projection.buildProjectionMatrixOrthoLH((f32)w, -(f32)h, -1.0f, 1.0f);
				m_projection.setTranslation(core::vector3df(-1, 1, 0));

				m_materialID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
			}

			CSkylichtRenderer::~CSkylichtRenderer()
			{
				delete m_fontLarge;
				delete m_fontNormal;
			}

			void CSkylichtRenderer::resize(float w, float h)
			{
				m_width = w;
				m_height = h;

				m_projection.buildProjectionMatrixOrthoLH((f32)w, -(f32)h, -1.0f, 1.0f);
				m_projection.setTranslation(core::vector3df(-1.0f, 1.0f, 0));
			}

			void CSkylichtRenderer::begin()
			{
				// clear z
				getVideoDriver()->clearZBuffer();

				CGraphics2D::getInstance()->beginRenderGUI(m_projection, m_view);

				// enable scissor
				getVideoDriver()->enableScissor(true);
			}

			void CSkylichtRenderer::end()
			{
				CGraphics2D::getInstance()->flush();

				// disable scissor
				getVideoDriver()->enableScissor(false);
			}

			void CSkylichtRenderer::startClip()
			{
				IVideoDriver *driver = getVideoDriver();

				CGraphics2D *g = CGraphics2D::getInstance();
				g->flush();

				driver->setScissor(core::recti(
					(s32)m_rectClipRegion.X,
					(s32)m_rectClipRegion.Y,
					(s32)(m_rectClipRegion.X + m_rectClipRegion.Width),
					(s32)(m_rectClipRegion.Y + m_rectClipRegion.Height))
				);
			}

			void CSkylichtRenderer::endClip()
			{

			}

			void CSkylichtRenderer::enableClip(bool b)
			{
				CGraphics2D *g = CGraphics2D::getInstance();
				g->flush();

				getVideoDriver()->enableScissor(b);
			}

			void CSkylichtRenderer::setProjection()
			{
				CGraphics2D::getInstance()->beginRenderGUI(m_projection, m_view);
			}

			void CSkylichtRenderer::flush()
			{
				CGraphics2D::getInstance()->flush();
			}

			void CSkylichtRenderer::drawFillRect(const SRect &r, const SGUIColor& color)
			{
				CGraphics2D *g = CGraphics2D::getInstance();
				CSkylichtTheme *theme = (CSkylichtTheme*)CTheme::getTheme();

				float invW = 1.0f / (float)theme->getAtlasWidth();
				float invH = 1.0f / (float)theme->getAtlasHeight();

				const core::matrix4 &world = getWorldTransform();

				float offsetX = 2.0f;
				float offsetY = 2.0f;
				core::rectf uv(offsetX * invW, offsetY * invH, offsetX * invW, offsetY * invH);

				g->addRectangleBatch(getRect(r), uv, getColor(color), world, m_materialID, NULL);
			}

			void CSkylichtRenderer::renderText(const SRect &r, EFontSize fontSize, const SGUIColor& textColor, const std::wstring& string)
			{
				CGlyphFont *font = m_fontNormal;
				if (fontSize == EFontSize::SizeLarge)
					font = m_fontLarge;

				CGraphics2D *g = CGraphics2D::getInstance();

				const core::matrix4 &world = getWorldTransform();

				std::vector<SModuleOffset*> modules;
				const wchar_t* lpString = string.c_str();

				int i = 0, n = 0;
				float charSpacePadding = 0.0f;
				float charPadding = 0.0f;
				float x = 0.0f;
				float y = 0.0f;
				SColor color(textColor.A, textColor.R, textColor.G, textColor.B);

				while (lpString[i] != 0)
				{
					if (lpString[i] != (int)'\n' &&
						lpString[i] != (int)'\r')
					{
						SModuleOffset *c = font->getCharacterModule((int)lpString[i]);
						if (c != NULL)
						{
							modules.push_back(c);
						}
					}
					i++;
				}

				for (i = 0, n = (int)modules.size(); i < n; i++)
				{
					SModuleOffset* moduleOffset = modules[i];

					g->addModuleBatch(moduleOffset, color, world, (float)x, (float)y, m_materialID);

					if (moduleOffset->Character == ' ')
						x += ((int)moduleOffset->XAdvance + charSpacePadding);
					else
						x += ((int)moduleOffset->XAdvance + charPadding);
				}
			}

			SDimension CSkylichtRenderer::measureText(EFontSize fontSize, const std::wstring& string)
			{
				CGlyphFont *font = m_fontNormal;
				if (fontSize == EFontSize::SizeLarge)
					font = m_fontLarge;

				float stringWidth = 0.0f;
				float charSpacePadding = 0.0f;
				float charPadding = 0.0f;
				float textHeight = 0.0f;
				float textOffsetY = 0.0f;

				// get text height
				SModuleOffset *moduleCharA = font->getCharacterModule((int)'A');
				if (moduleCharA)
				{
					textHeight = moduleCharA->OffsetY + moduleCharA->Module->H;
					textOffsetY = moduleCharA->OffsetY;
				}

				// get text width
				std::vector<SModuleOffset*>	listModule;

				int i = 0, n;
				while (string[i] != 0)
				{
					SModuleOffset *c = font->getCharacterModule((int)string[i]);
					if (c != NULL)
					{
						listModule.push_back(c);
					}
					i++;
				}

				for (i = 0, n = (int)listModule.size(); i < n; i++)
				{
					SModuleOffset* moduleOffset = listModule[i];
					if (moduleOffset->Character == ' ')
						stringWidth = stringWidth + (moduleOffset->XAdvance + charSpacePadding);
					else
						stringWidth = stringWidth + (moduleOffset->XAdvance + charPadding);
				}

				return SDimension(stringWidth, textHeight + textOffsetY);
			}

			void CSkylichtRenderer::initFont(CSpriteAtlas *atlas)
			{
				CGlyphFreetype *glyphFreetype = CGlyphFreetype::getInstance();
				glyphFreetype->initFont(CThemeConfig::FontName.c_str(), CThemeConfig::FontPath.c_str());

				io::IReadFile* charlist = getIrrlichtDevice()->getFileSystem()->createAndOpenFile("BuiltIn/Fonts/character-list.txt");
				u32 filesize = charlist->getSize();
				u8 *data = new u8[filesize + 2];
				memset(data, 0, filesize + 2);
				charlist->read(data, filesize);
				charlist->drop();
				data[filesize] = 0;
				data[filesize + 1] = 0;

				wchar_t *unicode = (wchar_t*)data;
				u32 unicodeSize = filesize / 2;
				float advance = 0.0f, x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, offsetX = 0, offsetY = 0;

				float fontSmall = CThemeConfig::getFontSizePt(EFontSize::SizeLarge);
				float fontNormal = CThemeConfig::getFontSizePt(EFontSize::SizeNormal);

				// Cache character to sprite
				for (u32 i = 0; i < unicodeSize; i++)
				{
					glyphFreetype->getCharImage(atlas,
						unicode[i],
						CThemeConfig::FontName.c_str(),
						CGlyphFreetype::sizePtToPx(fontSmall),
						&advance,
						&x, &y, &w, &h,
						&offsetX, &offsetY);

					glyphFreetype->getCharImage(atlas,
						unicode[i],
						CThemeConfig::FontName.c_str(),
						CGlyphFreetype::sizePtToPx(fontNormal),
						&advance,
						&x, &y, &w, &h,
						&offsetX, &offsetY);
				}
				delete data;

				m_fontLarge = new CGlyphFont(CThemeConfig::FontName.c_str(), fontSmall);
				m_fontNormal = new CGlyphFont(CThemeConfig::FontName.c_str(), fontNormal);
			}

			core::rectf CSkylichtRenderer::getRect(const SRect& rect)
			{
				return core::rectf(rect.X, rect.Y, rect.X + rect.Width, rect.Y + rect.Height);
			}

			video::SColor CSkylichtRenderer::getColor(const SGUIColor& color)
			{
				return SColor(color.A, color.R, color.G, color.B);
			}
		}
	}
}