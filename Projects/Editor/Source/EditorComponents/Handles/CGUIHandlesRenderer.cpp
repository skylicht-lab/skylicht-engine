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
#include "CGUIHandlesRenderer.h"
#include "Handles/CGUIHandles.h"
#include "HitTest2D/CHitTest2D.h"

namespace Skylicht
{
	namespace Editor
	{
		const float DefaultLength = 100.0f;
		const float DefaultArrowSize1 = 10.0f;
		const float DefaultArrowSize2 = 5.0f;
		const float DefaultRectSize = 20.0f;

		CGUIHandlesRenderer::CGUIHandlesRenderer() :
			m_mouseState(-1),
			m_scale(1.0f),
			m_cancel(false),
			m_mouseDown(false)
		{
			m_directionColor[0].set(0xFF0000AA);
			m_directionColor[1].set(0xFFAA0000);
			m_directionColor[2].set(0xFF00AA00);

			m_hoverOnAxis[0] = false;
			m_hoverOnAxis[1] = false;
			m_hoverOnAxis[2] = false;

			m_selectionColor.set(0xFF1080FF);
		}

		CGUIHandlesRenderer::~CGUIHandlesRenderer()
		{

		}

		void CGUIHandlesRenderer::render(CCamera* camera, float scale)
		{
			CGUIHandles* handles = CGUIHandles::getInstance();
			m_scale = scale;

			core::vector3df pos = handles->getHandlePosition();
			handles->getWorld().transformVect(pos);

			core::quaternion worldRot(handles->getWorld());
			worldRot.normalize();

			core::quaternion rot = worldRot * handles->getHandleRotation();
			rot.normalize();

			if (handles->isHandlePosition())
			{
				drawTranslateGizmo(pos, rot);
			}
			else if (handles->isHandleRotation())
			{
				drawRotationGizmo(pos, rot);
			}
			else if (handles->isHandleScale())
			{
				drawScaleGizmo(pos, rot);
			}

			if (handles->isHandleRect())
			{
				core::matrix4 world = getHandleWorldTransform();
				drawRectGizmo(world);
			}
		}

		void CGUIHandlesRenderer::drawRotationGizmo(const core::vector3df& pos, const core::quaternion& rot)
		{
		}

		void CGUIHandlesRenderer::drawScaleGizmo(const core::vector3df& pos, const core::quaternion& rot)
		{
		}

		void CGUIHandlesRenderer::drawTranslateGizmo(const core::vector3df& pos, const core::quaternion& rot)
		{
			CGraphics2D* g = CGraphics2D::getInstance();
			float length = DefaultLength / m_scale;
			float arrowSize1 = DefaultArrowSize1 / m_scale;
			float arrowSize2 = DefaultArrowSize2 / m_scale;
			float rs = DefaultRectSize / m_scale;

			core::vector3df x(1.0f, 0.0f, 0.0f);
			core::vector3df sx(0.0f, 1.0f, 0.0f);
			x = rot * x;
			x.normalize();
			sx = rot * sx;
			sx.normalize();

			core::position2df dirAxisX(x.X, x.Y);
			core::position2df side(sx.X, sx.Y);

			// x			
			core::position2df p(pos.X, pos.Y);

			core::position2df a = p + dirAxisX * length;
			core::position2df m = a - dirAxisX * arrowSize1;
			core::position2df b = m + side * arrowSize2;
			core::position2df c = m - side * arrowSize2;

			g->draw2DLine(
				p,
				p + dirAxisX * length,
				m_hoverOnAxis[0] ? m_selectionColor : m_directionColor[0]);
			g->draw2DTriangle(a, b, c,
				m_hoverOnAxis[0] ? m_selectionColor : m_directionColor[0]);

			// y
			core::vector3df y(0.0f, 1.0f, 0.0f);
			core::vector3df sy(1.0f, 0.0f, 0.0f);
			y = rot * y;
			y.normalize();
			sy = rot * sy;
			sy.normalize();

			core::position2df dirAxisY(y.X, y.Y);
			side.set(sy.X, sy.Y);

			a = p + dirAxisY * length;
			m = a - dirAxisY * arrowSize1;
			b = m + side * arrowSize2;
			c = m - side * arrowSize2;

			g->draw2DLine(
				p,
				p + dirAxisY * length,
				m_hoverOnAxis[1] ? m_selectionColor : m_directionColor[1]);
			g->draw2DTriangle(a, b, c,
				m_hoverOnAxis[1] ? m_selectionColor : m_directionColor[1]);


			// draw square
			SColor fillColor = m_hoverOnAxis[2] ? m_selectionColor : m_directionColor[2];
			fillColor.setAlpha(50);
			core::vector3df r(rs, rs, 0.0f);
			r = rot * r;
			a = p;
			b = p + dirAxisX * rs;
			c.set(p.X + r.X, p.Y + r.Y);
			core::position2df d = p + dirAxisY * rs;

			g->draw2DTriangle(a, b, c, fillColor);
			g->draw2DTriangle(a, c, d, fillColor);

			std::vector<core::position2df> points;
			points.push_back(a);
			points.push_back(b);
			points.push_back(c);
			points.push_back(d);
			points.push_back(a);
			g->draw2DLines(points, m_hoverOnAxis[2] ? m_selectionColor : m_directionColor[2]);
		}

		void CGUIHandlesRenderer::drawRectGizmo(const core::matrix4& world)
		{
			CGraphics2D* g = CGraphics2D::getInstance();
			if (m_hoverOnRect[0] ||
				m_hoverOnRect[1] ||
				m_hoverOnRect[2] ||
				m_hoverOnRect[3])
			{
				float z = 0.0f;
				const core::rectf& r = CGUIHandles::getInstance()->getHandleRect();

				core::vector3df topLeft(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, z);
				core::vector3df topRight(r.LowerRightCorner.X, r.UpperLeftCorner.Y, z);
				core::vector3df bottomLeft(r.UpperLeftCorner.X, r.LowerRightCorner.Y, z);
				core::vector3df bottomRight(r.LowerRightCorner.X, r.LowerRightCorner.Y, z);

				world.transformVect(topLeft);
				world.transformVect(topRight);
				world.transformVect(bottomLeft);
				world.transformVect(bottomRight);

				core::vector2df tl(topLeft.X, topLeft.Y);
				core::vector2df tr(topRight.X, topRight.Y);
				core::vector2df br(bottomRight.X, bottomRight.Y);
				core::vector2df bl(bottomLeft.X, bottomLeft.Y);

				if (m_hoverOnRect[0])
					g->draw2DLine(tl, tr, m_selectionColor);

				if (m_hoverOnRect[1])
					g->draw2DLine(tr, br, m_selectionColor);

				if (m_hoverOnRect[2])
					g->draw2DLine(br, bl, m_selectionColor);

				if (m_hoverOnRect[3])
					g->draw2DLine(bl, tl, m_selectionColor);
			}
		}

		void CGUIHandlesRenderer::onMouseEvent(float x, float y, int state)
		{
			CGUIHandles* handles = CGUIHandles::getInstance();
			if (handles->isHandlePosition())
				handleTranslate(x, y, state);
			else if (handles->isHandleRotation())
				handleRotation(x, y, state);
			else if (handles->isHandleScale())
				handleScale(x, y, state);

			if (handles->isHandleRect())
				handleRect(x, y, state);
		}

		void CGUIHandlesRenderer::cancel()
		{
			m_cancel = true;
			m_mouseDown = false;

			CGUIHandles* handles = CGUIHandles::getInstance();
			if (handles->isHandlePosition())
			{
				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
				}
				handles->setTargetPosition(m_lastPosition);
			}
			else if (handles->isHandleRotation())
			{
				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
				}
				handles->setTargetRotation(m_lastRotate);
			}
			else if (handles->isHandleScale())
			{
				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
				}
				handles->setTargetRotation(m_lastScale);
			}
		}

		void CGUIHandlesRenderer::handleTranslate(float mouseX, float mouseY, int state)
		{
			CGUIHandles* handles = CGUIHandles::getInstance();
			core::vector3df mouse((float)mouseX, (float)mouseY, 0.0f);

			if (m_mouseState != state)
			{
				if (state == 1)
				{
					// mouse down
					m_mouseDown = true;
					m_lastMouse = mouse;
					m_lastPosition = handles->getHandlePosition();
					m_lastRect = handles->getHandleRect();
				}
				else if (state == 2)
				{
					// mouse up
					if (!m_cancel)
					{
						m_mouseDown = false;
						handles->setEndCheck(true);
					}
					m_cancel = false;
				}
			}
			m_mouseState = state;

			if (!m_cancel)
			{
				core::vector3df pos = handles->getHandlePosition();
				handles->getWorld().transformVect(pos);

				core::quaternion worldRot(handles->getWorld());
				worldRot.normalize();

				core::quaternion rot = worldRot * handles->getHandleRotation();
				rot.normalize();

				core::vector3df x(1.0f, 0.0f, 0.0f);
				core::vector3df y(0.0f, 1.0f, 0.0f);
				core::vector3df xy(1.0f, 1.0f, 0.0f);
				x = rot * x;
				x.normalize();

				y = rot * y;
				y.normalize();

				xy = rot * xy;

				if (m_mouseDown == false)
				{
					// check hover
					float length = DefaultLength / m_scale;
					float rs = DefaultRectSize / m_scale;

					core::position2df dirAxisX(x.X, x.Y);
					core::position2df dirAxisY(y.X, y.Y);
					core::position2df dirAxisXY(xy.X, xy.Y);
					core::position2df p(pos.X, pos.Y);

					core::position2df p1 = p + dirAxisX * length;
					core::position2df p2 = p + dirAxisY * length;

					core::position2df mousePoint(mouseX, mouseY);

					m_hoverOnAxis[0] = CHitTest2D::isLineHit(p, p1, mousePoint);
					m_hoverOnAxis[1] = CHitTest2D::isLineHit(p, p2, mousePoint);

					p1 = p + dirAxisX * rs;
					p2 = p + dirAxisXY * rs;
					core::position2df p3 = p + dirAxisY * rs;

					m_hoverOnAxis[2] = CHitTest2D::isInsidePoly(p, p1, p2, p3, mousePoint);
					if (m_hoverOnAxis[2])
					{
						m_hoverOnAxis[0] = false;
						m_hoverOnAxis[1] = false;
					}
				}
				else
				{
					if (m_hoverOnAxis[0] || m_hoverOnAxis[1] || m_hoverOnAxis[2])
					{
						// dragging
						core::vector3df offset = mouse - m_lastMouse;

						// convert to world
						core::vector3df resultPosition = m_lastPosition;
						handles->getWorld().transformVect(resultPosition);

						if (m_hoverOnAxis[2])
							resultPosition = resultPosition + offset;
						else if (m_hoverOnAxis[0])
						{
							float d = offset.dotProduct(x);
							resultPosition = resultPosition + x * d;
						}
						else if (m_hoverOnAxis[1])
						{
							float d = offset.dotProduct(y);
							resultPosition = resultPosition + y * d;
						}

						// convert to local
						handles->getWorldInv().transformVect(resultPosition);

						// snapVec3(resultPosition);
						handles->setTargetPosition(resultPosition);
					}
				}
			}
		}

		void CGUIHandlesRenderer::handleRotation(float mouseX, float mouseY, int state)
		{

		}

		void CGUIHandlesRenderer::handleScale(float mouseX, float mouseY, int state)
		{

		}

		void CGUIHandlesRenderer::handleRect(float mouseX, float mouseY, int state)
		{
			CGUIHandles* handles = CGUIHandles::getInstance();
			core::vector2df mouse((float)mouseX, (float)mouseY);

			if (!m_cancel)
			{
				core::matrix4 world = getHandleWorldTransform();

				const core::rectf& r = handles->getHandleRect();
				float z = 0.0f;

				core::vector3df topLeft(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, z);
				core::vector3df topRight(r.LowerRightCorner.X, r.UpperLeftCorner.Y, z);
				core::vector3df bottomLeft(r.UpperLeftCorner.X, r.LowerRightCorner.Y, z);
				core::vector3df bottomRight(r.LowerRightCorner.X, r.LowerRightCorner.Y, z);

				world.transformVect(topLeft);
				world.transformVect(topRight);
				world.transformVect(bottomLeft);
				world.transformVect(bottomRight);

				core::vector2df tl(topLeft.X, topLeft.Y);
				core::vector2df tr(topRight.X, topRight.Y);
				core::vector2df br(bottomRight.X, bottomRight.Y);
				core::vector2df bl(bottomLeft.X, bottomLeft.Y);

				if (m_mouseDown == false)
				{
					if (m_hoverOnAxis[0] || m_hoverOnAxis[1] || m_hoverOnAxis[2])
					{
						m_hoverOnRect[0] = false;
						m_hoverOnRect[1] = false;
						m_hoverOnRect[2] = false;
						m_hoverOnRect[3] = false;
					}
					else
					{
						m_hoverOnRect[0] = CHitTest2D::isLineHit(tl, tr, mouse);
						m_hoverOnRect[1] = CHitTest2D::isLineHit(tr, br, mouse);
						m_hoverOnRect[2] = CHitTest2D::isLineHit(br, bl, mouse);
						m_hoverOnRect[3] = CHitTest2D::isLineHit(bl, tl, mouse);
					}
				}
				else
				{
					// drag

				}
			}
		}

		core::matrix4 CGUIHandlesRenderer::getHandleWorldTransform()
		{
			CGUIHandles* handles = CGUIHandles::getInstance();
			const core::vector3df& pos = handles->getHandlePosition();
			const core::quaternion& rot = handles->getHandleRotation();
			const core::vector3df& scale = handles->getHandleScale();

			core::matrix4 mat = rot.getMatrix();
			f32* m = mat.pointer();

			m[0] *= scale.X;
			m[1] *= scale.X;
			m[2] *= scale.X;
			m[3] *= scale.X;

			m[4] *= scale.Y;
			m[5] *= scale.Y;
			m[6] *= scale.Y;
			m[7] *= scale.Y;

			m[8] *= scale.Z;
			m[9] *= scale.Z;
			m[10] *= scale.Z;
			m[11] *= scale.Z;

			m[12] = pos.X;
			m[13] = pos.Y;
			m[14] = pos.Z;

			return handles->getWorld() * mat;
		}
	}
}