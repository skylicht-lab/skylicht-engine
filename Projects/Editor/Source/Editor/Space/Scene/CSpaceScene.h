/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "SkylichtEngine.h"
#include "Editor/Space/CSpace.h"
#include "EditorComponents/Viewpoint/CViewpoint.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceScene : public CSpace
		{
		protected:
			GUI::CBase* m_3dPanel;

			CScene* m_scene;
			CCamera* m_editorCamera;
			IRenderPipeline* m_renderRP;
			IRenderPipeline* m_viewpointRP;

			CGameObject* m_gridPlane;

			bool m_leftMouseDown;
			bool m_rightMouseDown;
			bool m_middleMouseDown;

			float m_mouseX;
			float m_mouseY;

			CViewpoint* m_viewpoint;
			CZone* m_viewpointZone;
			CCamera* m_viewpointCamera;
			
			core::rectf m_viewpointRect;

		public:
			CSpaceScene(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceScene();

			void initDefaultScene();

			virtual void onResize(float w, float h);

			virtual void onRender(GUI::CBase* base);

			virtual void onMouseMoved(GUI::CBase* base, float x, float y, float deltaX, float deltaY);

			virtual void onLeftMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onRightMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onMiddleMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onMouseWheeled(GUI::CBase* base, int wheel);

			virtual void update();

			bool isEditorObject(CGameObject* object);

		protected:

			void updateViewpoint();

			void initRenderPipeline(float w, float h);

			void postMouseEventToScene(EMOUSE_INPUT_EVENT eventType, float x, float y);
		};
	}
}