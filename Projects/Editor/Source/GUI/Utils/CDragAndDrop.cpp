/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CDragAndDrop.h"
#include "GUI/Input/CInput.h"
#include "GUI//Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			static float s_mouseX;
			static float s_mouseY;
			static float s_lastMousePositionX = 0;
			static float s_lastMousePositionY = 0;
			static GUI::CBase* s_lastPressedControl = NULL;
			static GUI::CBase* s_lastHoverControl = NULL;
			static bool s_dragging = false;

			void CDragAndDrop::onControlDeleted(GUI::CBase* control)
			{
				if (control == s_lastHoverControl)
					s_lastHoverControl = NULL;

				if (control == s_lastPressedControl)
				{
					s_lastPressedControl = NULL;
					s_dragging = false;
				}
			}

			bool CDragAndDrop::onMouseButton(GUI::CBase* hoveredControl, float x, float y, bool down)
			{
				if (!down)
				{
					bool ret = false;

					if (s_lastPressedControl != NULL)
					{
						// mouse up
						bool accept = false;

						if (hoveredControl->OnAcceptDragDrop != nullptr)
							accept = hoveredControl->OnAcceptDragDrop(s_lastPressedControl->getDragDropPackage());

						if (hoveredControl->OnDrop != nullptr && accept)
							hoveredControl->OnDrop(s_lastPressedControl->getDragDropPackage(), x, y);

						ret = true;
					}

					s_lastMousePositionX = x;
					s_lastMousePositionY = y;
					s_lastPressedControl = NULL;
					s_lastHoverControl = NULL;
					s_dragging = false;

					return ret;
				}

				SDragDropPackage* drag = hoveredControl->getDragDropPackage();
				if (drag == NULL)
					return false;

				if (hoveredControl->OnShouldDrag != nullptr && hoveredControl->OnShouldDrag() == false)
					return false;

				s_mouseX = x;
				s_mouseY = y;
				s_lastMousePositionX = x;
				s_lastMousePositionY = y;
				s_lastPressedControl = hoveredControl;
				s_lastHoverControl = NULL;
				return false;
			}

			bool CDragAndDrop::onMouseMoved(GUI::CBase* hoveredControl, float x, float y)
			{
				if (s_lastPressedControl == NULL)
					return false;

				float iLength = std::abs(x - s_lastMousePositionX) + std::abs(y - s_lastMousePositionY);
				if (iLength < 5.0f)
					return false;

				if (s_dragging == false)
				{
					if (s_lastPressedControl->OnStartDragging != nullptr)
						s_lastPressedControl->OnStartDragging(s_lastPressedControl->getDragDropPackage(), x, y);

					s_lastPressedControl->onStartDragging(x, y);
				}

				bool accept = false;
				s_dragging = true;

				if (s_lastHoverControl != NULL && s_lastHoverControl != hoveredControl)
				{
					if (s_lastHoverControl->OnDragDropOut != nullptr)
						s_lastHoverControl->OnDragDropOut(s_lastPressedControl->getDragDropPackage(), x, y);
				}

				s_lastHoverControl = hoveredControl;

				if (s_lastPressedControl != hoveredControl)
				{
					if (hoveredControl->OnAcceptDragDrop != nullptr)
						accept = hoveredControl->OnAcceptDragDrop(s_lastPressedControl->getDragDropPackage());

					if (accept && hoveredControl->OnDragDropHover != nullptr)
						hoveredControl->OnDragDropHover(s_lastPressedControl->getDragDropPackage(), x, y);
				}

				if (!accept)
					CInput::getInput()->setCursor(GUI::ECursorType::No);
				else
					CInput::getInput()->setCursor(GUI::ECursorType::Normal);

				s_mouseX = x;
				s_mouseY = y;

				return true;
			}

			void CDragAndDrop::onRenderOverlay(GUI::CCanvas* canvas, CRenderer* renderer)
			{
				if (s_dragging && s_lastPressedControl != NULL && s_lastPressedControl->getDragDropPackage()->DrawControl != NULL)
				{
					SDragDropPackage* dragDrop = s_lastPressedControl->getDragDropPackage();

					CRenderer* renderer = CRenderer::getRenderer();
					SPoint offset = renderer->getRenderOffset();

					renderer->setRenderOffset(SPoint(
						s_mouseX - dragDrop->HoldOffsetX,
						s_mouseY - dragDrop->HoldOffsetY)
					);
					dragDrop->DrawControl->doRender();

					renderer->setRenderOffset(offset);
				}
			}
		}
	}
}