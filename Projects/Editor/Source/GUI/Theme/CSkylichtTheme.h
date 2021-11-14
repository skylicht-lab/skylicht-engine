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
				CSpriteAtlas* m_sprite;

				SFrame* m_empty;

				SFrame* m_window;
				SFrame* m_windowShadow;

				SFrame* m_button;
				SFrame* m_buttonShadow;

				SFrame* m_tabButton;
				SFrame* m_tabButtonFocus;

				SFrame* m_scrollbarH;
				SFrame* m_scrollbarV;

				SFrame* m_textboxShadow;
				SFrame* m_textbox;
				SFrame* m_textboxBorder;
				SFrame* m_textboxButtonLeft;
				SFrame* m_textboxButtonRight;
				SFrame* m_textboxButtonArrowLeft;
				SFrame* m_textboxButtonArrowRight;
				SFrame* m_textboxButtonArrowDown;

				SFrame* m_dockIcon[NumDockIcon];

				SFrame* m_systemIcon16[NumSystemIcon];
				SFrame* m_systemIcon32[NumSystemIcon];

				CGraphics2D* m_graphics;
				CSkylichtRenderer* m_renderer;

				int m_materialID;

			public:
				CSkylichtTheme();

				virtual ~CSkylichtTheme();

				virtual void drawIcon(const SRect& r, ESystemIcon icon, const SGUIColor& color, bool use32Bit);

				virtual void drawDockHintIcon(const SRect& r, EDockHintIcon icon, const SGUIColor& color);

				virtual void drawWindowShadow(const SRect& rect);

				virtual void drawWindow(const SRect& rect, bool isFocussed);

				virtual void drawButtonShadow(const SRect& rect);

				virtual void drawButton(const SRect& rect, const SGUIColor& color);

				virtual void drawTextBox(const SRect& rect, const SGUIColor& color);

				virtual void drawTextBoxButton(const SRect& rect, const SGUIColor& color, const SGUIColor& iconColor, bool left, bool right);

				virtual void drawTextBoxBorder(const SRect& rect, const SGUIColor& color);

				virtual void drawComboBoxButton(const SRect& rect, const SGUIColor& color, const SGUIColor& iconColor);

				virtual void drawTabButton(const SRect& rect, const SGUIColor& color, const SGUIColor& focusColor, bool focus);

				virtual void drawScrollbar(const SRect& rect, const SGUIColor& color, bool isHorizontal);

			public:

				SFrame* getEmptyFrame()
				{
					return m_empty;
				}

				SFrame* getSystemIcon16(ESystemIcon icon)
				{
					return m_systemIcon16[icon];
				}

				SFrame* getSystemIcon32(ESystemIcon icon)
				{
					return m_systemIcon32[icon];
				}

				inline int getAtlasWidth()
				{
					return m_sprite->getWidth();
				}

				inline int getAtlasHeight()
				{
					return m_sprite->getHeight();
				}

			private:

				void initDockHintIcon();

				void initSystemIcon();

				void addSystemIcon(ESystemIcon type, const char* name);

				void addEditorIcon(ESystemIcon type, const char* name);

				void drawGUIModule(SFrame* frame, const SRect& rect, const SGUIColor& color, float left, float top, float right, float bottom, float cornerRadius);

				core::rectf getRect(const SRect& rect);

				video::SColor getColor(const SGUIColor& color);

			};
		}
	}
}