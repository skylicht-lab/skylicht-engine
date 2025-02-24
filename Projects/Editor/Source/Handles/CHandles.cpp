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
#include "CHandles.h"
#include "Scene/CScene.h"
#include "GizmosComponents/Handles/CHandlesRenderer.h"
#include "GizmosComponents/Gizmos/CGizmosRenderer.h"
#include "Editor/SpaceController/CSceneController.h"

#include "Selection/CSelecting.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CHandles);

		CHandles::CHandles() :
			m_handlePosition(false),
			m_handleRotation(false),
			m_handleScale(false),
			m_mouseState(0),
			m_endCheck(false),
			m_useLocalSpace(true),
			m_handlesRenderer(NULL),
			m_gizmosRenderer(NULL),
			m_snapXZ(false),
			m_snapY(false),
			m_snapRotate(false),
			m_snapDistanceXZ(1.0f),
			m_snapDistanceY(1.0f),
			m_snapRotateDeg(10.0f),
			m_isAltPressed(false)
		{

		}

		CHandles::~CHandles()
		{

		}

		bool CHandles::isEnable()
		{
			if (m_handlesRenderer)
				return m_handlesRenderer->isEnable();
			return false;
		}

		void CHandles::setNullRenderer()
		{
			m_handlesRenderer = NULL;
			m_gizmosRenderer = NULL;
		}

		void CHandles::refresh()
		{
			CScene* scene = CSceneController::getInstance()->getScene();
			m_handlesRenderer = scene->getEntityManager()->getSystem<CHandlesRenderer>();
			m_gizmosRenderer = scene->getEntityManager()->getSystem<CGizmosRenderer>();
		}

		void CHandles::end()
		{
			m_handlePosition = false;
			m_handleRotation = false;
			m_handleScale = false;
		}

		void CHandles::reset()
		{
			end();
			m_handlesRenderer->cancel();
			m_mouseState = 0;
		}

		bool CHandles::endCheck()
		{
			bool ret = m_endCheck;
			if (m_endCheck == true)
				m_endCheck = false;
			return ret;
		}

		bool CHandles::isHoverOnAxisOrPlane()
		{
			if (m_handlesRenderer)
				return m_handlesRenderer->isHoverOnAxisOrPlane();
			return false;
		}

		bool CHandles::isUsing()
		{
			if (m_handlesRenderer)
				return m_handlesRenderer->isUsing();
			return false;
		}

		core::vector3df CHandles::positionHandle(const core::vector3df& position, const core::quaternion& localRotation)
		{
			if (!m_handlePosition)
				m_targetPosition = position;

			m_handlePosition = true;
			m_handleRotation = false;
			m_handleScale = false;

			m_position = m_targetPosition;
			m_rotation = localRotation;

			return m_targetPosition;
		}

		core::vector3df CHandles::scaleHandle(const core::vector3df& scale, const core::vector3df& origin, const core::quaternion& localRotation)
		{
			if (!m_handleScale)
				m_targetScale = scale;

			m_handleRotation = false;
			m_handlePosition = false;
			m_handleScale = true;

			m_rotation = localRotation;
			m_position = origin;
			m_scale = m_targetScale;
			return m_targetScale;
		}

		core::quaternion CHandles::rotateHandle(const core::quaternion& rotate, const core::vector3df& origin)
		{
			if (!m_handleRotation)
				m_targetRotation = rotate;

			m_handleRotation = true;
			m_handlePosition = false;
			m_handleScale = false;

			m_position = origin;
			m_rotation = m_targetRotation;
			return m_targetRotation;
		}

		bool CHandles::OnEvent(const SEvent& event)
		{
			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				int mouseX = event.MouseInput.X;
				int mouseY = event.MouseInput.Y;

				bool isDragSelect = CSelecting::getInstance()->isDragSelect();
				if (isDragSelect)
				{
					m_handlesRenderer->cancel();
					m_mouseState = 0;
				}

				if (m_handlesRenderer != NULL && !m_isAltPressed && !isDragSelect)
				{
					if (event.MouseInput.Event == EMIE_MOUSE_MOVED)
					{
						if (m_mouseState == 2)
							m_mouseState = 0;
						m_handlesRenderer->onMouseEvent(mouseX, mouseY, m_mouseState);
					}
					else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
					{
						m_mouseState = 1;
						m_handlesRenderer->onMouseEvent(mouseX, mouseY, m_mouseState);
					}
					else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
					{
						m_mouseState = 2;
						m_handlesRenderer->onMouseEvent(mouseX, mouseY, m_mouseState);
					}
					return true;
				}
			}
			else if (event.EventType == EET_KEY_INPUT_EVENT)
			{
				if (event.KeyInput.Key == irr::KEY_MENU ||
					event.KeyInput.Key == irr::KEY_LMENU ||
					event.KeyInput.Key == irr::KEY_RMENU)
				{
					m_isAltPressed = event.KeyInput.PressedDown;
					if (m_isAltPressed)
					{
						m_handlesRenderer->skip();
						m_mouseState = 0;
					}
					return true;
				}
				else if (event.KeyInput.Key == irr::KEY_ESCAPE)
				{
					if (event.KeyInput.PressedDown)
					{
						if (m_mouseState == 1)
						{
							m_handlesRenderer->cancel();
						}
						return true;
					}
				}
			}
			return false;
		}

		void CHandles::draw3DBox(const core::aabbox3d<f32>& box, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			m_gizmosRenderer->getLineData()->add3DBox(box, color);
		}

		void CHandles::drawCylinder(const core::vector3df& pos, const core::vector3df& direction, float length, float radius, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			CLineDrawData* linedata = m_gizmosRenderer->getLineData();

			core::quaternion q;
			q.rotationFromTo(core::vector3df(0.0f, 1.0f, 0.0f), direction);

			int numStep = 12;
			float a = core::PI * 2.0f / (float)numStep;
			for (int i = 0; i < numStep; i++)
			{
				float angle1 = i * a;
				float angle2 = (i + 1) * a;

				core::vector3df p1 = radius * core::vector3df(cosf(angle1), 0.0f, sinf(angle1));
				core::vector3df p2 = radius * core::vector3df(cosf(angle2), 0.0f, sinf(angle2));
				p1 = pos + q * p1;
				p2 = pos + q * p2;

				core::vector3df t = direction * length * 0.5f;

				linedata->addLine(p1 + t, p2 + t, color);
				linedata->addLine(p1 - t, p2 - t, color);
				linedata->addLine(p1 + t, p1 - t, color);
			}
		}

		void CHandles::drawSphere(const core::vector3df& pos, float radius, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			core::vector3df normal;

			normal = core::vector3df(0.0f, 1.0f, 0.0f);
			drawCircle(pos, radius, normal, color);

			normal = core::vector3df(0.0f, 0.0f, 1.0f);
			drawCircle(pos, radius, normal, color);
		}

		void CHandles::drawLine(const core::vector3df& v1, const core::vector3df& v2, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			m_gizmosRenderer->getLineData()->addLine(v1, v2, color);
		}

		void CHandles::drawPolyline(const core::vector3df* points, u32 count, bool close, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			m_gizmosRenderer->getLineData()->addPolyline(points, count, close, color);
		}

		void CHandles::drawCircle(const core::vector3df& pos, float radius, const core::vector3df& normal, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			core::quaternion q;
			core::vector3df up(0.0f, 1.0f, 0.0f);
			core::vector3df n = normal;
			n.normalize();
			q.rotationFromTo(up, n);

			int step = 30;
			float rad = 0;
			float radInc = core::PI * 2 / step;

			core::vector3df	point;
			core::array<core::vector3df> points;

			for (int i = 0; i <= step; i++)
			{
				point.Y = 0.0;
				point.X = radius * sinf(rad);
				point.Z = radius * cosf(rad);

				// rotate
				point = q * point;

				// translate
				point += pos;

				// add point
				points.push_back(point);

				// inc rad
				rad = rad + radInc;
			}

			m_gizmosRenderer->getLineData()->addPolyline(points.const_pointer(), points.size(), true, color);
		}

		void CHandles::drawRectBillboard(const core::vector3df& pos, float w, float h, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			m_gizmosRenderer->drawRectBillboard(pos, w, h, color);
		}

		void CHandles::drawArrowInViewSpace(const core::vector3df& pos, const core::vector3df& v, float length, float arrowSize, const SColor& color)
		{
			if (m_gizmosRenderer == NULL)
				return;

			m_gizmosRenderer->drawArrowInViewSpace(pos, v, length, arrowSize, color);
		}
	}
}