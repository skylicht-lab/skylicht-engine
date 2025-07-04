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
#include "Editor/Space/Scene/CDiffuseLightRenderPipeline.h"
#include "Editor/Space/Scene/CNullForwarderPipeline.h"

#include "GizmosComponents/Handles/CHandlesRenderer.h"
#include "GizmosComponents/Gizmos/CGizmosRenderer.h"
#include "GizmosComponents/SelectObject/CSelectObjectSystem.h"

#include "Selection/CSelectingRenderer.h"

#include "CViewpointController.h"
#include "CCameraSettingController.h"
#include "CSnapSettingController.h"

#include "Reactive/ISubject.h"
#include "Reactive/IObserver.h"

#include "History/CSceneHistory.h"

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
			Snap,
			Perspective,
			Ortho,
			Pipeline,
			ToolbarCount
		};

		class CSpaceScene :
			public CSpace,
			public IObserver
		{
		protected:
			GUI::CBase* m_view;

			CScene* m_scene;
			CCamera* m_editorCamera;

			IRenderPipeline* m_renderRP;
			IRenderPipeline* m_viewpointRP;

			CDeferredRP* m_rendering;
			CShadowMapRP* m_shadowMapRendering;
			CForwardRP* m_forwardRP;
			CPostProcessorRP* m_postProcessor;
			CDiffuseLightRenderPipeline* m_lightRP;
			CNullForwarderPipeline* m_nullRP;

			CGameObject* m_gridPlane;

			bool m_leftMouseDown;
			bool m_rightMouseDown;
			bool m_middleMouseDown;

			float m_mouseX;
			float m_mouseY;

			core::dimension2df m_renderSize;
			core::recti m_sceneRect;

			CViewpoint* m_viewpoint;
			core::rectf m_viewpointRect;
			CZone* m_viewpointZone;
			CCamera* m_viewpointCamera;
			CViewpointController* m_viewpointController;

			GUI::CMenu* m_snapSettingMenu;
			CSnapSettingController* m_snapSettingController;

			GUI::CMenu* m_cameraSettingMenu;
			CCameraSettingController* m_cameraSettingController;

			CHandlesRenderer* m_handlesRenderer;
			CGizmosRenderer* m_gizmosRenderer;
			CSelectingRenderer* m_selectingRenderer;

			bool m_enableHandles;

			CSelectObjectSystem* m_selectObjectSystem;

			GUI::CButton* m_toolbarButton[ToolbarCount];

			GUI::CToggleGroup* m_groupTransform;
			GUI::CToggleGroup* m_groupCameraView;

			bool m_enableRender;
			bool m_enableRenderGrid;

			bool m_waitHotKeyRelease;

			enum ERenderPipeline
			{
				Materials = 0,
				Lighting
			};

			ERenderPipeline m_rp;

		public:
			CSpaceScene(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceScene();

			void initEditorObject(CZone* zone);

			void initDefaultScene();

			void reinitCurrentScene();

			CScene* initNullScene();

			virtual void onResize(float w, float h);

			virtual void onRender(GUI::CBase* base);

			virtual void onMouseMoved(GUI::CBase* base, float x, float y, float deltaX, float deltaY);

			virtual void onLeftMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onRightMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onMiddleMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onMouseWheeled(GUI::CBase* base, int wheel);

			virtual void onHotkey(GUI::CBase* base, const std::string& hotkey);

			virtual void onKeyPressed(GUI::CBase* base, int key, bool down);

			virtual void onRenderResize(GUI::CBase* base);

			virtual void onNotify(ISubject* subject, IObserver* from);

			void onToolbarTransform(GUI::CBase* base);

			void onToolbarWorldSpace(GUI::CBase* base);

			void onToolbarPipeline(GUI::CBase* base);

			void onSnapSetting(GUI::CBase* base);

			void onCameraSetting(GUI::CBase* base);

			void onCameraPerspective(GUI::CBase* base);

			void onCameraOrtho(GUI::CBase* base);

			virtual void update();

			virtual void refresh();

			inline void enableRender(bool b)
			{
				m_enableRender = b;
			}

			void enableRenderGrid(bool b);

			inline bool isEnableRenderGrid()
			{
				return m_enableRenderGrid;
			}

			inline CSelectObjectSystem* getSelectObjectSystem()
			{
				return m_selectObjectSystem;
			}

			inline IRenderPipeline* getRenderPipeline()
			{
				return m_renderRP;
			}

			inline void enablePostProcessing(bool b)
			{
				if (m_rendering && m_postProcessor)
					m_rendering->setPostProcessor(b ? m_postProcessor : NULL);
			}

			CCamera* getEditorCamera()
			{
				return m_editorCamera;
			}

			void updateSnapButton();

		protected:

			void initRenderPipeline(float w, float h);

			void postMouseEventToScene(EMOUSE_INPUT_EVENT eventType, float x, float y);
		};
	}
}