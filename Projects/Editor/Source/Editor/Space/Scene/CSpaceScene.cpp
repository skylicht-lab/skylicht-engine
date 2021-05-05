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

#include "pch.h"
#include "CSpaceScene.h"
#include "GUI/Input/CInput.h"
#include "GridPlane/CGridPlane.h"
#include "EditorComponents/Viewpoint/CViewpoint.h"

#include "Editor/SpaceController/CSceneController.h"

using namespace std::placeholders;

namespace Skylicht
{
	namespace Editor
	{
		CSpaceScene::CSpaceScene(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_scene(NULL),
			m_renderRP(NULL),
			m_viewpointRP(NULL),
			m_editorCamera(NULL),
			m_gridPlane(NULL),
			m_leftMouseDown(false),
			m_rightMouseDown(false),
			m_middleMouseDown(false),
			m_mouseX(0.0f),
			m_mouseY(0.0f),
			m_viewpointZone(NULL),
			m_viewpoint(NULL),
			m_viewpointCamera(NULL),
			m_3dPanel(NULL)
		{
			initDefaultScene();

			GUI::CBase* panel = m_window->getInnerPanel();

			panel->OnRender = BIND_LISTENER(&CSpaceScene::onRender, this);
			panel->OnMouseMoved = std::bind(&CSpaceScene::onMouseMoved, this, _1, _2, _3, _4, _5);
			panel->OnLeftMouseClick = std::bind(&CSpaceScene::onLeftMouseClick, this, _1, _2, _3, _4);
			panel->OnRightMouseClick = std::bind(&CSpaceScene::onRightMouseClick, this, _1, _2, _3, _4);
			panel->OnMiddleMouseClick = std::bind(&CSpaceScene::onMiddleMouseClick, this, _1, _2, _3, _4);
			panel->OnMouseWheeled = std::bind(&CSpaceScene::onMouseWheeled, this, _1, _2);

			m_3dPanel = panel;

			GUI::SDimension size = window->getSize();
			initRenderPipeline(size.Width, size.Height);

			CSceneController::getInstance()->setSpaceScene(this);
		}

		CSpaceScene::~CSpaceScene()
		{
			if (m_scene != NULL)
				delete m_scene;

			if (m_renderRP != NULL)
				delete m_renderRP;

			if (m_viewpointRP != NULL)
				delete m_viewpointRP;

			CSceneController* sceneController = CSceneController::getInstance();
			sceneController->setScene(NULL);
			sceneController->setSpaceScene(NULL);
		}

		void CSpaceScene::initDefaultScene()
		{
			// create a scene
			m_scene = new CScene();

			// create a zone in scene
			CZone* zone = m_scene->createZone();

			// create editor camera
			CGameObject* camObj = zone->createEmptyObject();
			camObj->setName(L"EditorCamera");
			camObj->setEditorObject(true);
			camObj->addComponent<CCamera>();
			camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

			m_editorCamera = camObj->getComponent<CCamera>();
			m_editorCamera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
			m_editorCamera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

			// grid
			m_gridPlane = zone->createEmptyObject();
			m_gridPlane->setName(L"Grid3D");
			m_gridPlane->setEditorObject(true);
			m_gridPlane->addComponent<CGridPlane>();

			// lighting
			CGameObject* lightObj = zone->createEmptyObject();
			lightObj->setName(L"DirectionLight");

			CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
			SColor c(255, 255, 244, 214);
			directionalLight->setColor(SColorf(c));

			CTransformEuler* lightTransform = lightObj->getTransformEuler();
			lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

			core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
			lightTransform->setOrientation(direction, CTransform::s_oy);


			// viewpoint zone
			m_viewpointZone = m_scene->createZone();
			m_viewpointZone->setEditorObject(true);

			// camera
			camObj = m_viewpointZone->createEmptyObject();
			camObj->setName(L"ViewpointCamera");
			camObj->setEditorObject(true);
			camObj->addComponent<CCamera>();

			m_viewpointCamera = camObj->getComponent<CCamera>();
			m_viewpointCamera->setAspect(1.0f);

			// viewpoint
			CGameObject* viewpoint = m_viewpointZone->createEmptyObject();
			viewpoint->setName(L"Viewpoint");
			m_viewpoint = viewpoint->addComponent<CViewpoint>();

			// set scene to controller
			CSceneController::getInstance()->setScene(m_scene);
		}

		void CSpaceScene::onResize(float w, float h)
		{
			initRenderPipeline(w, h);
		}

		void CSpaceScene::initRenderPipeline(float w, float h)
		{
			if (w >= 32.0f && h >= 32.0f)
			{
				if (m_renderRP == NULL)
				{
					// rendering pipe line
					m_renderRP = new CForwardRP();
					m_renderRP->initRender((int)w, (int)h);
				}
				else
				{
					// resize
					m_renderRP->resize((int)w, (int)h);
				}

				if (m_viewpointRP == NULL)
				{
					// rendering pipe line
					m_viewpointRP = new CForwardRP(false);
					m_viewpointRP->initRender((int)w, (int)h);
				}
				else
				{
					m_viewpointRP->resize((int)w, (int)h);
				}
			}
		}

		void CSpaceScene::update()
		{
			updateViewpoint();

			m_scene->update();
		}

		void CSpaceScene::updateViewpoint()
		{
			const core::matrix4& view = m_editorCamera->getViewMatrix();

			core::matrix4 cameraWorld;
			view.getInverse(cameraWorld);
			f32* matData = cameraWorld.pointer();

			core::vector3df dir(matData[8], matData[9], matData[10]);
			dir.normalize();

			core::vector3df up(matData[4], matData[5], matData[6]);
			up.normalize();

			float distance = 2.0f;
			core::vector3df pos = -dir * distance;

			m_viewpointCamera->setPosition(pos);
			m_viewpointCamera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), up);
		}

		void CSpaceScene::onRender(GUI::CBase* base)
		{
			if (m_renderRP != NULL)
			{
				// flush 2d gui
				GUI::CGUIContext::getRenderer()->flush();
				core::recti vp = getVideoDriver()->getViewPort();
				getVideoDriver()->enableScissor(false);
				getVideoDriver()->clearZBuffer();

				// setup scene viewport
				GUI::SPoint position = base->localPosToCanvas();
				core::recti viewport;
				viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
				viewport.LowerRightCorner.set((int)(position.X + base->width()), (int)(position.Y + base->height()));

				// draw scene
				m_scene->setVisibleAllZone(true);
				m_viewpointZone->setVisible(false);

				m_renderRP->render(NULL, m_editorCamera, m_scene->getEntityManager(), viewport);

				// setup viewpoint viewport
				int paddingTop = 10;
				int paddingLeft = 10;
				int viewpointSize = 125;
				viewport.UpperLeftCorner.set((int)(position.X + base->width()) - viewpointSize - paddingLeft, (int)position.Y + paddingTop);
				viewport.LowerRightCorner = viewport.UpperLeftCorner + core::vector2di(viewpointSize, viewpointSize);

				// viewpoint rect (local)
				GUI::SPoint upleft = m_3dPanel->canvasPosToLocal(GUI::SPoint((float)viewport.UpperLeftCorner.X, (float)viewport.UpperLeftCorner.Y));
				GUI::SPoint lowerRight = m_3dPanel->canvasPosToLocal(GUI::SPoint((float)viewport.LowerRightCorner.X, (float)viewport.LowerRightCorner.Y));
				m_viewpointRect.UpperLeftCorner.set(upleft.X, upleft.Y);
				m_viewpointRect.LowerRightCorner.set(lowerRight.X, lowerRight.Y);
				m_viewpointRect.repair();

				getVideoDriver()->clearZBuffer();

				// draw viewpoint
				m_scene->setVisibleAllZone(false);
				m_viewpointZone->setVisible(true);

				m_viewpointRP->render(NULL, m_viewpointCamera, m_scene->getEntityManager(), viewport);

				// disable viewpoint
				m_scene->setVisibleAllZone(true);
				m_viewpointZone->setVisible(false);

				// resume gui render
				getVideoDriver()->enableScissor(true);
				getVideoDriver()->setViewPort(vp);
				GUI::CGUIContext::getRenderer()->setProjection();
			}
		}

		void CSpaceScene::onMouseMoved(GUI::CBase* base, float x, float y, float deltaX, float deltaY)
		{
			GUI::SPoint local = m_3dPanel->canvasPosToLocal(GUI::SPoint(x, y));
			postMouseEventToScene(EMIE_MOUSE_MOVED, local.X, local.Y);
		}

		void CSpaceScene::onLeftMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_leftMouseDown = down;
			GUI::SPoint local = m_3dPanel->canvasPosToLocal(GUI::SPoint(x, y));

			if (m_viewpointRect.isPointInside(core::vector2df(local.X, local.Y)))
			{
				if (down)
				{
					// viewpoint
					float x = local.X - m_viewpointRect.UpperLeftCorner.X;
					float y = local.Y - m_viewpointRect.UpperLeftCorner.Y;
					CViewpointData::EAxis hit = m_viewpoint->getViewpointData()->hit(m_viewpointCamera, x, y, (int)m_viewpointRect.getWidth(), (int)m_viewpointRect.getHeight());
					if (hit >= CViewpointData::X)
					{

					}
				}
			}
			else
			{
				// scene
				if (down)
				{
					GUI::CInput::getInput()->setCapture(m_3dPanel);
					postMouseEventToScene(EMIE_LMOUSE_PRESSED_DOWN, local.X, local.Y);
				}
				else
				{
					GUI::CInput::getInput()->setCapture(NULL);
					postMouseEventToScene(EMIE_LMOUSE_LEFT_UP, local.X, local.Y);
				}
			}
		}

		void CSpaceScene::onRightMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_rightMouseDown = down;
			GUI::SPoint local = m_3dPanel->canvasPosToLocal(GUI::SPoint(x, y));

			if (m_viewpointRect.isPointInside(core::vector2df(local.X, local.Y)))
			{
				// viewpoint
			}
			else
			{
				// scene
				if (down)
				{
					GUI::CInput::getInput()->setCapture(m_3dPanel);
					postMouseEventToScene(EMIE_RMOUSE_PRESSED_DOWN, local.X, local.Y);
				}
				else
				{
					GUI::CInput::getInput()->setCapture(NULL);
					postMouseEventToScene(EMIE_RMOUSE_LEFT_UP, local.X, local.Y);
				}
			}
		}

		void CSpaceScene::onMiddleMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_middleMouseDown = down;
			GUI::SPoint local = m_3dPanel->canvasPosToLocal(GUI::SPoint(x, y));

			if (m_viewpointRect.isPointInside(core::vector2df(local.X, local.Y)))
			{
				// viewpoint
			}
			else
			{
				// scene
				if (down)
				{
					GUI::CInput::getInput()->setCapture(m_3dPanel);
					postMouseEventToScene(EMIE_MMOUSE_PRESSED_DOWN, local.X, local.Y);
				}
				else
				{
					GUI::CInput::getInput()->setCapture(NULL);
					postMouseEventToScene(EMIE_MMOUSE_LEFT_UP, local.X, local.Y);
				}
			}
		}

		void CSpaceScene::onMouseWheeled(GUI::CBase* base, int wheel)
		{
			if (m_viewpointRect.isPointInside(core::vector2df(m_mouseX, m_mouseY)))
			{
				// viewpoint
			}
			else
			{
				// scene
				SEvent event;
				event.EventType = EET_MOUSE_INPUT_EVENT;
				event.MouseInput.Event = EMIE_MOUSE_WHEEL;
				event.MouseInput.X = (int)m_mouseX;
				event.MouseInput.Y = (int)m_mouseY;
				event.MouseInput.Wheel = (float)wheel;
				event.MouseInput.ButtonStates = 0;

				if (m_leftMouseDown)
					event.MouseInput.ButtonStates |= EMBSM_LEFT;

				if (m_rightMouseDown)
					event.MouseInput.ButtonStates |= EMBSM_RIGHT;

				m_scene->OnEvent(event);
			}
		}

		void CSpaceScene::postMouseEventToScene(EMOUSE_INPUT_EVENT eventType, float x, float y)
		{
			m_mouseX = x;
			m_mouseY = y;

			SEvent event;
			event.EventType = EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = eventType;
			event.MouseInput.X = (int)m_mouseX;
			event.MouseInput.Y = (int)m_mouseY;
			event.MouseInput.Wheel = 0.0f;
			event.MouseInput.ButtonStates = 0;

			if (m_leftMouseDown)
				event.MouseInput.ButtonStates |= EMBSM_LEFT;

			if (m_rightMouseDown)
				event.MouseInput.ButtonStates |= EMBSM_RIGHT;

			m_scene->OnEvent(event);
		}

		bool CSpaceScene::isEditorObject(CGameObject* object)
		{
			if (m_editorCamera->getGameObject() == object ||
				m_gridPlane == object)
			{
				return true;
			}

			return false;
		}
	}
}