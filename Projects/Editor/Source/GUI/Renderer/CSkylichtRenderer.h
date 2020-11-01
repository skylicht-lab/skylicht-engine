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
#pragma once

#include "CRenderer.h"
#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"
#include "Graphics2D/SpriteFrame/CGlyphFont.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CSkylichtRenderer : public CRenderer
			{
			protected:

				core::matrix4 m_projection;
				core::matrix4 m_view;
				core::matrix4 m_world;

				float m_width;
				float m_height;

				int m_materialID;

				CGlyphFont *m_fontLarge;
				CGlyphFont *m_fontNormal;

			public:
				CSkylichtRenderer(float w, float h);

				virtual ~CSkylichtRenderer();

				virtual void resize(float w, float h);

				virtual void begin();

				virtual void end();

				virtual void startClip();

				virtual void endClip();

				virtual void enableClip(bool b);

				virtual void drawFillRect(const SRect &r, const SGUIColor& color);

				virtual void renderText(const SRect &r, EFontSize fontSize, const SGUIColor& textColor, const std::wstring& string);

				virtual SDimension measureText(EFontSize fontSize, const std::wstring& string);

				void initFont(CSpriteAtlas *atlas);

				const core::matrix4& getWorldTransform()
				{
					m_world.setTranslation(core::vector3df(m_renderOffset.X, m_renderOffset.Y, 0.0f));
					return m_world;
				}

				core::rectf getRect(const SRect& rect);

				video::SColor getColor(const SGUIColor& color);
			};
		}
	}
}