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

#include "pch.h"
#include "CSpaceGUIDesign.h"
#include "GUI/Input/CInput.h"

#include "Editor/SpaceController/CSceneController.h"
#include "Editor/SpaceController/CGUIDesignController.h"

#include "Handles/CGUIHandles.h"

#include "Graphics2D/CGUIImporter.h"

using namespace std::placeholders;

namespace Skylicht
{
	namespace Editor
	{
		const float LeftOffset = 40.0f;// == size the height TOP Ruler

		CSpaceGUIDesign::CSpaceGUIDesign(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_guiWidth(1920.0f),
			m_guiHeight(1080.0f),
			m_guiScale(1.0f),
			m_viewX(0.0f),
			m_viewY(0.0f),
			m_pressX(0.0f),
			m_pressY(0.0f),
			m_mouseGUIX(0.0f),
			m_mouseGUIY(0.0f),
			m_leftMouseDown(false),
			m_rightMouseDown(false),
			m_middleDrag(false),
			m_scene(NULL),
			m_guiCamera(NULL)
		{
			GUI::CButton* btn = NULL;
			m_toolBar = new GUI::CToolbar(window);
			m_toolBar->addButton(L"New", GUI::ESystemIcon::NewFile);
			m_toolBar->addButton(L"Save", GUI::ESystemIcon::Save);
			m_toolBar->addSpace();

			btn = m_toolBar->addButton(L"Zoom in", GUI::ESystemIcon::ZoomIn);
			btn->OnPress = BIND_LISTENER(&CSpaceGUIDesign::onZoomIn, this);

			btn = m_toolBar->addButton(L"Zoom out", GUI::ESystemIcon::ZoomOut);
			btn->OnPress = BIND_LISTENER(&CSpaceGUIDesign::onZoomOut, this);

			m_toolBar->addSpace();
			m_toolBar->addButton(L"Copy", GUI::ESystemIcon::Copy);
			m_toolBar->addButton(L"Paste", GUI::ESystemIcon::Paste);

			m_toolBar->addSpace();
			btn = m_toolBar->addButton(L"Setting", GUI::ESystemIcon::Setting, true);
			m_canvasSettingMenu = new GUI::CMenu(window->getCanvas());
			m_canvasSettingController = new CCanvasSettingController(editor, m_canvasSettingMenu);
			btn->OnPress = BIND_LISTENER(&CSpaceGUIDesign::onSetting, this);
			
			m_textMousePos = new GUI::CLabel(m_toolBar);
			m_toolBar->addControl(m_textMousePos, true);
			m_textMousePos->setPadding(GUI::SPadding(0.0f, 3.0f, 0.0f, 0.0f));
			m_textMousePos->setString(L"(0, 0)");
			m_textMousePos->setTextAlignment(GUI::TextCenter);
			m_textMousePos->setWidth(90.0f);

			m_textZoom = new GUI::CLabel(m_toolBar);
			m_toolBar->addControl(m_textZoom, true);
			m_textZoom->setPadding(GUI::SPadding(0.0f, 3.0f, 0.0f, 0.0f));
			m_textZoom->setString(L"Zoom: 100%");
			m_textZoom->sizeToContents();

			m_leftRuler = new GUI::CRulerBar(window, false);
			m_leftRuler->setWidth(60.0f);
			m_leftRuler->setPixelPerUnit(10.0f);

			m_topRuler = new GUI::CRulerBar(window, true);
			m_topRuler->setPixelPerUnit(10.0f);

			m_leftRuler->setUnitScale(10.0f);
			m_topRuler->setUnitScale(10.0f);

			m_topRuler->setBeginOffset(0.0f);
			m_leftRuler->setBeginOffset(LeftOffset);

			m_topRuler->enableDrawCursorline(true);
			m_leftRuler->enableDrawCursorline(true);

			m_view = new GUI::CBase(window);
			m_view->dock(GUI::EPosition::Fill);
			m_view->enableRenderFillRect(true);
			m_view->setFillRectColor(GUI::CThemeConfig::WindowBackgroundColor);
			m_view->setKeyboardInputEnabled(true);

			m_view->OnMouseWheeled = std::bind(&CSpaceGUIDesign::onMouseWheel, this, _1, _2);
			m_view->OnMouseMoved = std::bind(&CSpaceGUIDesign::onMouseMoved, this, _1, _2, _3, _4, _5);
			m_view->OnMiddleMouseClick = std::bind(&CSpaceGUIDesign::onMiddleMouseClick, this, _1, _2, _3, _4);
			m_view->OnLeftMouseClick = std::bind(&CSpaceGUIDesign::onLeftMouseClick, this, _1, _2, _3, _4);
			m_view->OnRightMouseClick = std::bind(&CSpaceGUIDesign::onRightMouseClick, this, _1, _2, _3, _4);
			m_view->OnKeyPress = std::bind(&CSpaceGUIDesign::onKeyPressed, this, _1, _2, _3);

			m_view->OnRender = BIND_LISTENER(&CSpaceGUIDesign::onRender, this);

			m_gizmos = new CGUITransformGizmos();

			m_handlesRenderer = new CGUIHandlesRenderer();
			CGUIHandles::getInstance()->setHandleRenderer(m_handlesRenderer);

			m_selecting = new CGUISelecting();

			CGUIDesignController::getInstance()->setSpaceDesign(this);
		}

		CSpaceGUIDesign::~CSpaceGUIDesign()
		{
			CGUIHandles::getInstance()->setHandleRenderer(NULL);

			delete m_selecting;
			delete m_handlesRenderer;
			delete m_gizmos;

			CGUIDesignController* controller = CGUIDesignController::getInstance();
			if (controller->getSpaceDesign() == this)
				controller->setSpaceDesign(NULL);
		}

		void CSpaceGUIDesign::update()
		{
			CSpace::update();
			m_gizmos->onGizmos();
		}

		void CSpaceGUIDesign::refresh()
		{
			m_gizmos->refresh();

			CGUIDesignController::getInstance()->refresh();
		}

		void CSpaceGUIDesign::openGUI(const char* path)
		{
			// fix for open new space and open path
			if (!m_scene)
				m_scene = CSceneController::getInstance()->getScene();

			if (m_scene)
			{
				CGameObject* canvasObj = m_scene->searchObjectInChild(L"GUICanvas");
				CCanvas* canvas = canvasObj->getComponent<CCanvas>();
				canvas->removeAllElement();
				m_gizmos->onRemove();

				if (CGUIImporter::beginImport(path, canvas) == true)
				{
					bool loadFinish = false;
					do
					{
						loadFinish = CGUIImporter::updateLoadGUI();
					} while (!loadFinish);

					CGUIDesignController::getInstance()->rebuildGUIHierachy();

					// reset view
					m_guiScale = 1.0f;
					m_viewX = 0.0f;
					m_viewY = 0.0f;

					float dx = 0.0f;
					float dy = 0.0f;
					m_topRuler->setPosition(-(m_viewX * m_guiScale + dx));
					m_leftRuler->setPosition(-(m_viewY * m_guiScale + dy));

					canvas->getRootElement()->setPosition(
						core::vector3df(
							m_viewX + dx / m_guiScale,
							m_viewY + dy / m_guiScale,
							0.0f)
					);
				}
			}
		}

		void CSpaceGUIDesign::onKeyPressed(GUI::CBase* base, int key, bool down)
		{
			GUI::CInput* input = GUI::CInput::getInput();

			SEvent event;
			event.EventType = EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)key;
			event.KeyInput.PressedDown = down;
			event.KeyInput.Control = input->IsControlDown();
			event.KeyInput.Shift = input->IsShiftDown();

			event.GameEvent.Sender = m_scene;

			CGUIHandles::getInstance()->OnEvent(event);

			if (down == true)
			{
				if (key == GUI::KEY_DELETE)
				{
					CGUIDesignController::getInstance()->onDelete();
				}
			}
		}

		void CSpaceGUIDesign::onMiddleMouseClick(GUI::CBase* view, float x, float y, bool down)
		{
			m_middleDrag = down;

			if (down)
			{
				GUI::CInput::getInput()->setCapture(view);
				m_pressX = x;
				m_pressY = y;
			}
			else
			{
				GUI::CInput::getInput()->setCapture(NULL);

				float dx = x - m_pressX;
				float dy = y - m_pressY;

				m_viewX = m_viewX + dx / m_guiScale;
				m_viewY = m_viewY + dy / m_guiScale;
			}
		}

		void CSpaceGUIDesign::onMouseMoved(GUI::CBase* view, float x, float y, float deltaX, float deltaY)
		{
			if (m_middleDrag)
			{
				float dx = x - m_pressX;
				float dy = y - m_pressY;

				m_topRuler->setPosition(-(m_viewX * m_guiScale + dx));
				m_leftRuler->setPosition(-(m_viewY * m_guiScale + dy));

				if (m_scene)
				{
					CGameObject* canvasObj = m_scene->searchObjectInChild(L"GUICanvas");
					CCanvas* canvas = canvasObj->getComponent<CCanvas>();
					canvas->getRootElement()->setPosition(
						core::vector3df(
							m_viewX + dx / m_guiScale,
							m_viewY + dy / m_guiScale,
							0.0f)
					);
				}
			}

			// check the mouse position
			GUI::SPoint localPos = view->canvasPosToLocal(GUI::SPoint(x, y));
			wchar_t mouseText[512];

			float mx = localPos.X - m_viewX * m_guiScale;
			float my = localPos.Y - m_viewY * m_guiScale;
			m_mouseGUIX = mx / m_guiScale;
			m_mouseGUIY = my / m_guiScale;
			m_mouseX = localPos.X;
			m_mouseY = localPos.Y;

			swprintf(
				mouseText, 512,
				L"(%d, %d)",
				(int)m_mouseGUIX,
				(int)m_mouseGUIY
			);
			m_textMousePos->setString(std::wstring(mouseText));

			m_topRuler->setCursorPosition(mx);
			m_leftRuler->setCursorPosition(my);

			postMouseEventToHandles(EMIE_MOUSE_MOVED);
		}

		void CSpaceGUIDesign::onMouseWheel(GUI::CBase* scroll, int delta)
		{
			GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
			GUI::SPoint localPos = scroll->canvasPosToLocal(mousePos);

			float dx = localPos.X / m_guiScale - m_viewX;
			float dy = localPos.Y / m_guiScale - m_viewY;

			if (delta < 0)
			{
				// zoom in
				if (m_guiScale < 4.0f)
				{
					doZoomIn(dx, dy);
				}
			}
			else
			{
				// zoom out
				if (m_guiScale > 0.25f)
				{
					doZoomOut(dx, dy);
				}
			}
		}

		void CSpaceGUIDesign::onLeftMouseClick(GUI::CBase* view, float x, float y, bool down)
		{
			m_leftMouseDown = down;
			if (down)
			{
				GUI::CInput::getInput()->setCapture(view);
				postMouseEventToHandles(EMIE_LMOUSE_PRESSED_DOWN);
			}
			else
			{
				GUI::CInput::getInput()->setCapture(NULL);
				postMouseEventToHandles(EMIE_LMOUSE_LEFT_UP);
			}
		}

		void CSpaceGUIDesign::onRightMouseClick(GUI::CBase* view, float x, float y, bool down)
		{
			m_rightMouseDown = down;
			if (down)
			{
				GUI::CInput::getInput()->setCapture(view);
				postMouseEventToHandles(EMIE_RMOUSE_PRESSED_DOWN);
			}
			else
			{
				GUI::CInput::getInput()->setCapture(NULL);
				postMouseEventToHandles(EMIE_RMOUSE_LEFT_UP);
			}
		}

		void CSpaceGUIDesign::postMouseEventToHandles(EMOUSE_INPUT_EVENT eventType)
		{
			SEvent event;
			event.EventType = EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = eventType;
			event.MouseInput.Wheel = 0.0f;
			event.MouseInput.ButtonStates = 0;

			if (m_leftMouseDown)
				event.MouseInput.ButtonStates |= EMBSM_LEFT;

			if (m_rightMouseDown)
				event.MouseInput.ButtonStates |= EMBSM_RIGHT;

			event.GameEvent.Sender = m_scene;

			// pick select gui object
			event.MouseInput.X = (int)m_mouseX;
			event.MouseInput.Y = (int)m_mouseY;
			m_selecting->OnEvent(event);

			// drag change position, rect
			event.MouseInput.X = (int)m_mouseGUIX;
			event.MouseInput.Y = (int)m_mouseGUIY;
			CGUIHandles::getInstance()->OnEvent(event);
		}

		void CSpaceGUIDesign::onZoomIn(GUI::CBase* base)
		{
			doZoomIn(0.0f, 0.0f);
		}

		void CSpaceGUIDesign::onZoomOut(GUI::CBase* base)
		{
			doZoomOut(0.0f, 0.0f);
		}

		void CSpaceGUIDesign::doZoomIn(float dx, float dy)
		{
			if (m_guiScale < 4.0f)
			{
				m_guiScale = m_guiScale * 2.0f;

				int z = (int)(m_guiScale * 100);
				wchar_t text[128];
				swprintf(text, 128, L"Zoom: %2d%%", z);
				m_textZoom->setString(text);

				m_leftRuler->setPixelPerUnit(10.0f * m_guiScale);
				m_topRuler->setPixelPerUnit(10.0f * m_guiScale);

				m_leftRuler->setTextPerUnit((int)(10.0f / m_guiScale));
				m_topRuler->setTextPerUnit((int)(10.0f / m_guiScale));

				m_viewX = m_viewX + dx;
				m_viewY = m_viewY + dy;

				m_viewX = m_viewX / 2.0f;
				m_viewY = m_viewY / 2.0f;

				m_viewX = m_viewX - dx;
				m_viewY = m_viewY - dy;

				m_topRuler->setPosition(-m_viewX * m_guiScale);
				m_leftRuler->setPosition(-m_viewY * m_guiScale);
			}

			if (m_scene)
			{
				CGameObject* canvasObj = m_scene->searchObjectInChild(L"GUICanvas");
				CCanvas* canvas = canvasObj->getComponent<CCanvas>();
				canvas->getRootElement()->setPosition(core::vector3df(m_viewX, m_viewY, 0.0f));
			}
		}

		void CSpaceGUIDesign::doZoomOut(float dx, float dy)
		{
			if (m_guiScale > 0.25f)
			{
				m_guiScale = m_guiScale * 0.5f;

				int z = (int)(m_guiScale * 100);
				wchar_t text[128];
				swprintf(text, 128, L"Zoom: %2d%%", z);
				m_textZoom->setString(text);

				m_leftRuler->setPixelPerUnit(10.0f * m_guiScale);
				m_topRuler->setPixelPerUnit(10.0f * m_guiScale);

				m_leftRuler->setTextPerUnit((int)(10.0f / m_guiScale));
				m_topRuler->setTextPerUnit((int)(10.0f / m_guiScale));

				m_viewX = m_viewX + dx;
				m_viewY = m_viewY + dy;

				m_viewX = m_viewX * 2.0f;
				m_viewY = m_viewY * 2.0f;

				m_viewX = m_viewX - dx;
				m_viewY = m_viewY - dy;

				m_topRuler->setPosition(-m_viewX * m_guiScale);
				m_leftRuler->setPosition(-m_viewY * m_guiScale);
			}

			if (m_scene)
			{
				CGameObject* canvasObj = m_scene->searchObjectInChild(L"GUICanvas");
				CCanvas* canvas = canvasObj->getComponent<CCanvas>();
				canvas->getRootElement()->setPosition(core::vector3df(m_viewX, m_viewY, 0.0f));
			}
		}

		void CSpaceGUIDesign::onSetting(GUI::CBase* base)
		{
			m_canvasSettingMenu->open(base);
			m_canvasSettingController->onShow();
		}
	
		void CSpaceGUIDesign::onRender(GUI::CBase* base)
		{
			// flush 2d gui
			GUI::CGUIContext::getRenderer()->flush();
			core::recti vp = getVideoDriver()->getViewPort();
			getVideoDriver()->enableScissor(false);
			getVideoDriver()->clearZBuffer();

			{
				// setup scene viewport
				GUI::SPoint position = m_view->localPosToCanvas();
				core::recti viewport;
				viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
				viewport.LowerRightCorner.set((int)(position.X + m_view->width()), (int)(position.Y + base->height()));

				// also apply viewport for pick selection
				m_selecting->setViewport((int)m_view->width(), (int)m_view->height());

				// setup new viewport
				getVideoDriver()->setViewPort(viewport);

				if (!m_scene)
					m_scene = CSceneController::getInstance()->getScene();

				if (m_scene)
				{
					if (!m_guiCamera)
						m_guiCamera = m_scene->searchObjectInChild(L"GUICamera")->getComponent<CCamera>();

					// update viewport size
					m_guiCamera->setOrthoUISize(
						m_view->width() / m_guiScale,
						m_view->height() / m_guiScale
					);
					m_guiCamera->enableCustomOrthoUISize(true);
					m_guiCamera->recalculateProjectionMatrix();

					// render GUI
					CGraphics2D::getInstance()->render(m_guiCamera);

					// render Gizmos
					m_handlesRenderer->render(m_guiCamera, m_guiScale);
				}
			}

			// resume gui render
			getVideoDriver()->enableScissor(true);
			getVideoDriver()->setViewPort(vp);
			GUI::CGUIContext::getRenderer()->setProjection();
		}
	}
}
