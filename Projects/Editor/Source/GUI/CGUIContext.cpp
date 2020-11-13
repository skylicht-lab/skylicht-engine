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
#include "CGUIContext.h"
#include "GUI/Controls/CCanvas.h"

#include "GUI/Renderer/CSkylichtRenderer.h"
#include "GUI/Theme/CSkylichtTheme.h"
#include "GUI/Input/CSkylichtInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CBase* CGUIContext::HoveredControl = NULL;
			CBase* CGUIContext::MouseFocus = NULL;
			CBase* CGUIContext::KeyboardFocus = NULL;

			CCanvas* g_rootCanvas = NULL;
			CInput* g_input = NULL;

			CSkylichtRenderer *g_renderer = NULL;
			CSkylichtTheme *g_theme = NULL;

			float g_guiUpdateTime = 0.0f;

			void CGUIContext::initGUI(float width, float height)
			{
				g_rootCanvas = new CCanvas(width, height);

				g_renderer = new CSkylichtRenderer(width, height);
				CRenderer::setRenderer(g_renderer);

				g_theme = new CSkylichtTheme();
				CTheme::setTheme(g_theme);

				g_input = new CSkylichtInput();
				CInput::setInput(g_input);
			}

			void CGUIContext::destroyGUI()
			{
				if (g_rootCanvas != NULL)
					delete g_rootCanvas;

				if (g_renderer != NULL)
					delete g_renderer;

				if (g_theme != NULL)
					delete g_theme;

				if (g_input != NULL)
					delete g_input;
			}

			void CGUIContext::update(float time)
			{
				if (g_rootCanvas != NULL)
					g_rootCanvas->update();

				g_guiUpdateTime = time;
			}

			float CGUIContext::getTime()
			{
				return g_guiUpdateTime;
			}

			void CGUIContext::render()
			{
				if (g_rootCanvas != NULL)
					g_rootCanvas->doRender();
			}

			void CGUIContext::resize(float width, float height)
			{
				if (g_renderer != NULL)
					g_renderer->resize(width, height);

				if (g_rootCanvas != NULL)
				{
					g_rootCanvas->setBounds(0.0f, 0.0f, width, height);
					g_rootCanvas->closeMenu();
					g_rootCanvas->notifySaveDockLayout();
				}
			}

			CCanvas* CGUIContext::getRoot()
			{
				return g_rootCanvas;
			}

			CRenderer* CGUIContext::getRenderer()
			{
				return g_renderer;
			}
		}
	}
}