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
#include "Editor/History/CSceneHistory.h"
#include "EditorComponents/Handles/CHandlesRenderer.h"
#include "CViewpointController.h"

#include "Reactive/ISubject.h"
#include "Reactive/IObserver.h"

namespace Skylicht
{
	namespace Editor
	{
		enum ESceneToolBar
		{
			Select = 0,
			Hand,
			Move,
			Rotate,
			Scale,
			World,
			Perspective,
			Ortho,
			ToolbarCount
		};

		class CSpaceScene :
			public CSpace,
			public IObserver
		{
		protected:
			GUI::CBase* m_view;

			CSceneHistory* m_history;

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

			core::recti m_sceneRect;

			CViewpoint* m_viewpoint;
			core::rectf m_viewpointRect;
			CZone* m_viewpointZone;
			CCamera* m_viewpointCamera;
			CViewpointController* m_viewpointController;

			CHandlesRenderer* m_handlesRenderer;

			GUI::CButton* m_toolbarButton[ToolbarCount];

			GUI::CToggleGroup* m_groupEditor;
			GUI::CToggleGroup* m_groupTransform;
			GUI::CToggleGroup* m_groupCameraView;

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

			virtual void onChar(GUI::CBase* base, u32 c);

			virtual void onKeyPressed(GUI::CBase* base, int key, bool down);

			virtual void onRenderResize(GUI::CBase* base);

			virtual void onNotify(ISubject* subject, IObserver* from);

			void onToolbarTransform(GUI::CBase* base);

			void onToolbarWorldSpace(GUI::CBase* base);

			void onCameraPerspective(GUI::CBase* base);

			void onCameraOrtho(GUI::CBase* base);

			void onEditorSelect(GUI::CBase* base);

			void onEditorHand(GUI::CBase* base);

			virtual void update();

			virtual void refresh();

			bool isEditorObject(CGameObject* object);

		protected:

			void initRenderPipeline(float w, float h);

			void postMouseEventToScene(EMOUSE_INPUT_EVENT eventType, float x, float y);
		};
	}
}