/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CInterpolateCurvesController.h"
#include "CValueSettingController.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceInterpolateCurves : public CSpace
		{
		public:

		protected:
			GUI::CBase* m_owner;
			GUI::CBase* m_view;

			GUI::CToolbar* m_toolBar;
			GUI::CLabel* m_textMousePos;

			CInterpolateCurvesController* m_controller;

			core::vector2df m_guiScale;
			float m_viewX;
			float m_viewY;

			core::vector2df m_gridSize;

			float m_pressX;
			float m_pressY;
			float m_oldViewX;
			float m_oldViewY;
			bool m_middleDrag;

			float m_mouseX;
			float m_mouseY;
			float m_mouseGUIX;
			float m_mouseGUIY;
			bool m_leftMouseDown;
			bool m_rightMouseDown;

			int m_hoverLine;
			int m_hoverPoint;
			int m_hoverState;

			GUI::CMenu* m_valueSettingMenu;
			CValueSettingController* m_valueSettingController;

			GUI::CMenu* m_insertPointMenu;
			int m_insertId;
			core::vector2df m_insertPosition;

			std::vector<ArrayPoint2df> m_lines;

		public:
			CSpaceInterpolateCurves(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceInterpolateCurves();

			virtual void onResize(float w, float h);

			virtual void onRender(GUI::CBase* base);

			inline CInterpolateCurvesController* getController()
			{
				return m_controller;
			}

			void setOwner(GUI::CBase* owner);

			inline GUI::CBase* getOwner()
			{
				return m_owner;
			}

			inline void autoZoom()
			{
				onAutoZoom(NULL);
			}

			void onOwnerClosed();

		protected:

			void onClear(GUI::CBase* base);

			void onLinear(GUI::CBase* base);

			void onInOutCubic(GUI::CBase* base);

			void onAutoZoom(GUI::CBase* base);

			void renderGrid();

			void renderGraph();

			void computeBezier(const SControlPoint& p1, const SControlPoint& p2, core::array<core::vector2df>& lines);

			void onMiddleMouseClick(GUI::CBase* view, float x, float y, bool down);

			void onMouseMoved(GUI::CBase* view, float x, float y, float deltaX, float deltaY);

			void onMouseWheel(GUI::CBase* view, int delta);

			void onLeftMouseClick(GUI::CBase* view, float x, float y, bool down);

			void onRightMouseClick(GUI::CBase* view, float x, float y, bool down);

			void onInsertCommand(GUI::CBase* base);

			void ptToView(float& x, float& y);

			void viewToPt(float& x, float& y);

			void doZoomIn(float dx, float dy);

			void doZoomOut(float dx, float dy);

			void checkHoverPoint();

			void doDragPoint();
		};
	}
}