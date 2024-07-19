/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#pragma once

#include "SkylichtEngine.h"
#include "Editor/Space/CSpace.h"
#include "EditorComponents/Handles/CGUIHandlesRenderer.h"
#include "Editor/Gizmos/GUITransform/CGUITransformGizmos.h"
#include "Selection/CGUISelecting.h"
#include "CCanvasSettingController.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceGUIDesign : public CSpace
		{
		protected:
			GUI::CToolbar* m_toolBar;
			GUI::CRulerBar* m_topRuler;
			GUI::CRulerBar* m_leftRuler;
			GUI::CBase* m_view;

			GUI::CLabel* m_textZoom;
			GUI::CLabel* m_textMousePos;

			float m_guiWidth;
			float m_guiHeight;
			float m_guiScale;

			float m_pressX;
			float m_pressY;
			float m_viewX;
			float m_viewY;

			float m_mouseX;
			float m_mouseY;
			float m_mouseGUIX;
			float m_mouseGUIY;
			bool m_leftMouseDown;
			bool m_rightMouseDown;

			bool m_middleDrag;

			CScene* m_scene;
			CCamera* m_guiCamera;

			CGUIHandlesRenderer* m_handlesRenderer;
			CGUITransformGizmos* m_gizmos;
			CGUISelecting* m_selecting;

			GUI::CMenu* m_canvasSettingMenu;
			CCanvasSettingController* m_canvasSettingController;
			
		public:
			CSpaceGUIDesign(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceGUIDesign();

			virtual void update();

			virtual void refresh();

			void openGUI(const char* path);

		protected:

			void onKeyPressed(GUI::CBase* base, int key, bool down);

			void onMiddleMouseClick(GUI::CBase* view, float x, float y, bool down);

			void onMouseMoved(GUI::CBase* view, float x, float y, float deltaX, float deltaY);

			void onMouseWheel(GUI::CBase* view, int delta);

			void onLeftMouseClick(GUI::CBase* base, float x, float y, bool down);

			void onRightMouseClick(GUI::CBase* base, float x, float y, bool down);

			void onZoomIn(GUI::CBase* base);

			void onZoomOut(GUI::CBase* base);

			void doZoomIn(float dx, float dy);

			void doZoomOut(float dx, float dy);

			void onSetting(GUI::CBase* base);
			
			void onRender(GUI::CBase* base);

			void postMouseEventToHandles(EMOUSE_INPUT_EVENT eventType);

		public:

			inline CGUITransformGizmos* getGizmos()
			{
				return m_gizmos;
			}
		};
	}
}
