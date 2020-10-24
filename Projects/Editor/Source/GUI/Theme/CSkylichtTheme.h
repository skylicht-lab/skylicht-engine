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

#include "CTheme.h"
#include "CThemeConfig.h"
#include "GUI/Renderer/CSkylichtRenderer.h"
#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"
#include "Graphics2D/CGraphics2D.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CSkylichtTheme : public CTheme
			{
			protected:
				CSpriteAtlas *m_sprite;
				SFrame *m_empty;
				SFrame *m_window;
				SFrame *m_windowShadow;

				CGraphics2D *m_graphics;
				CSkylichtRenderer *m_renderer;

				int m_materialID;

			public:
				CSkylichtTheme();

				virtual ~CSkylichtTheme();

				virtual void drawWindowShadow(const SRect& rect);

				virtual void drawWindow(const SRect& rect, bool isFocussed);

			private:

				core::rectf getRect(const SRect& rect);

				video::SColor getColor(const SGUIColor& color);

			};
		}
	}
}