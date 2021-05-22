/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

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
#include "CHandlesRenderer.h"
#include "Handles/CHandles.h"
#include "Entity/CEntityManager.h"
#include "Projective/CProjective.h"

namespace Skylicht
{
	namespace Editor
	{
		CHandlesRenderer::CHandlesRenderer() :
			m_screenFactor(1.0f),
			m_allowAxisFlip(true),
			m_camera(NULL),
			m_mouseDown(false),
			m_mouseState(0)
		{
			m_data = new CHandlesData();
			m_data->LineBuffer->getMaterial().ZBuffer = ECFN_DISABLED;
			m_data->LineBuffer->getMaterial().ZWriteEnable = false;

			m_data->PolygonBuffer->getMaterial().ZBuffer = ECFN_DISABLED;
			m_data->PolygonBuffer->getMaterial().ZWriteEnable = false;

			m_directionUnary[0] = core::vector3df(1.f, 0.f, 0.f);
			m_directionUnary[1] = core::vector3df(0.f, 1.f, 0.f);
			m_directionUnary[2] = core::vector3df(0.f, 0.f, 1.f);

			m_directionColor[0].set(0xFF0000AA);
			m_directionColor[1].set(0xFFAA0000);
			m_directionColor[2].set(0xFF00AA00);

			m_selectionColor.set(0xFF1080FF);

			for (int i = 0; i < 3; i++)
			{
				m_axisFactor[i] = 1.0f;
				m_belowAxisLimit[i] = true;
				m_belowPlaneLimit[i] = true;
				m_hoverOnAxis[i] = false;
				m_hoverOnPlane[i] = false;
			}
		}

		CHandlesRenderer::~CHandlesRenderer()
		{
			delete m_data;
		}

		void CHandlesRenderer::beginQuery()
		{

		}

		void CHandlesRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
		{

		}

		void CHandlesRenderer::init(CEntityManager* entityManager)
		{

		}

		void CHandlesRenderer::update(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			((CLineDrawData*)m_data)->clearBuffer();
			((CPolygonDrawData*)m_data)->clearBuffer();

			// Caculate the screen scale factor
			irr::core::matrix4 invView;
			{
				irr::core::matrix4 view(getVideoDriver()->getTransform(video::ETS_VIEW));
				view.getInversePrimitive(invView);
			}

			core::vector3df cameraRight(invView[0], invView[1], invView[2]);
			core::vector3df cameraLook(invView[8], invView[9], invView[10]);
			core::vector3df cameraUp(invView[4], invView[5], invView[6]);
			core::vector3df cameraPos = entityManager->getCamera()->getGameObject()->getPosition();

			cameraRight.normalize();
			cameraLook.normalize();
			cameraUp.normalize();

			CCamera* camera = entityManager->getCamera();

			// Draw position axis
			CHandles* handles = CHandles::getInstance();
			if (handles->isHandlePosition())
			{
				const core::vector3df& pos = handles->getHandlePosition();

				m_screenFactor = 0.2f / getSegmentLengthClipSpace(pos, pos + cameraRight, camera);

				drawTranslateGizmo(pos, cameraPos, camera);
				//drawRotationGizmo(pos, cameraPos);				
				//drawScaleGizmo(pos, cameraLook, cameraUp, camera);
			}

			((CLineDrawData*)m_data)->updateBuffer();
			((CPolygonDrawData*)m_data)->updateBuffer();
		}

		void CHandlesRenderer::drawRotationGizmo(const core::vector3df& pos, const core::vector3df& cameraPos)
		{
			core::vector3df cameraToObject = pos - cameraPos;
			cameraToObject.normalize();

			const int halfCircleSegmentCount = 64;
			const int circleMul = 1;

			core::vector3df* circlePos = new core::vector3df[circleMul * halfCircleSegmentCount + 1];

			for (int axis = 0; axis < 3; axis++)
			{
				// draw rotation
				float* p = &cameraToObject.X;
				float angleStart = atan2f(p[(4 - axis) % 3], p[(3 - axis) % 3]) + core::PI * 0.5f;

				for (unsigned int i = 0; i < circleMul * halfCircleSegmentCount + 1; i++)
				{
					float ng = angleStart + core::PI * ((float)i / (float)halfCircleSegmentCount);

					core::vector3df axisPos = core::vector3df(cosf(ng), sinf(ng), 0.f);
					p = &axisPos.X;

					circlePos[i] = core::vector3df(p[axis], p[(axis + 1) % 3], p[(axis + 2) % 3]) * m_screenFactor;
				}

				// draw circle
				m_data->addPolyline(circlePos, circleMul * halfCircleSegmentCount, false, m_hoverOnAxis[axis] ? m_selectionColor : m_directionColor[axis]);
			}

			delete[]circlePos;
		}

		void CHandlesRenderer::drawScaleGizmo(const core::vector3df& pos, const core::vector3df& cameraLook, const core::vector3df& cameraUp, CCamera* camera)
		{
			for (int i = 0; i < 3; i++)
			{
				core::vector3df dirAxis, dirPlaneX, dirPlaneY;
				bool belowAxisLimit, belowPlaneLimit;

				computeTripodAxisAndVisibility(i, pos, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit, camera);

				if (belowAxisLimit)
				{
					m_scaleAxis[i].start = pos;
					m_scaleAxis[i].end = pos + dirAxis * m_screenFactor;

					// draw axis
					m_data->addLine(m_scaleAxis[i].start, m_scaleAxis[i].end, m_hoverOnAxis[i] ? m_selectionColor : m_directionColor[i]);

					// draw quad
					float size = m_screenFactor * 0.05f;

					core::vector3df side = cameraUp.crossProduct(cameraLook);
					side.normalize();

					core::vector3df a = pos + dirAxis * m_screenFactor;

					core::vector3df sideQuad = side * size;
					core::vector3df upQuad = cameraUp * size;

					core::vector3df v1, v2, v3, v4;
					v1.set(
						a.X - sideQuad.X + upQuad.X,
						a.Y - sideQuad.Y + upQuad.Y,
						a.Z - sideQuad.Z + upQuad.Z
					);
					v2.set(
						a.X + sideQuad.X + upQuad.X,
						a.Y + sideQuad.Y + upQuad.Y,
						a.Z + sideQuad.Z + upQuad.Z
					);
					v3.set(
						a.X + sideQuad.X - upQuad.X,
						a.Y + sideQuad.Y - upQuad.Y,
						a.Z + sideQuad.Z - upQuad.Z
					);
					v4.set(
						a.X - sideQuad.X - upQuad.X,
						a.Y - sideQuad.Y - upQuad.Y,
						a.Z - sideQuad.Z - upQuad.Z
					);
					core::vector3df quad[] = { v1, v2, v3, v4 };
					m_data->addPolygonFill(quad, 4, m_hoverOnAxis[i] ? m_selectionColor : m_directionColor[i]);
				}
			}
		}

		void CHandlesRenderer::drawTranslateGizmo(const core::vector3df& pos, const core::vector3df& cameraPos, CCamera* camera)
		{
			float quadMin = 0.1f;
			float quadMax = 0.4f;

			core::vector3df quad[4] = {
				core::vector3df(quadMin, quadMin, 0.0f),
				core::vector3df(quadMin, quadMax, 0.0f),
				core::vector3df(quadMax, quadMax, 0.0f),
				core::vector3df(quadMax, quadMin, 0.0f),
			};

			for (int i = 0; i < 3; i++)
			{
				core::vector3df dirAxis, dirPlaneX, dirPlaneY;
				bool belowAxisLimit, belowPlaneLimit;

				computeTripodAxisAndVisibility(i, pos, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit, camera);

				if (belowAxisLimit)
				{
					// save the position
					m_translateAxis[i].start = pos;
					m_translateAxis[i].end = pos + dirAxis * m_screenFactor;

					// draw axis
					m_data->addLine(m_translateAxis[i].start, m_translateAxis[i].end, m_hoverOnAxis[i] ? m_selectionColor : m_directionColor[i]);

					// draw arrow
					float arrowSize1 = m_screenFactor * 0.1f;
					float arrowSize2 = m_screenFactor * 0.05f;

					core::vector3df look = cameraPos - pos;
					look.normalize();

					core::vector3df side = dirAxis.crossProduct(look);
					side.normalize();

					core::vector3df a = pos + dirAxis * m_screenFactor;
					core::vector3df m = a - dirAxis * arrowSize1;
					core::vector3df b = m + side * arrowSize2;
					core::vector3df c = m - side * arrowSize2;
					m_data->addTriangleFill(a, b, c, m_hoverOnAxis[i] ? m_selectionColor : m_directionColor[i]);
				}

				if (belowPlaneLimit)
				{
					// draw plane
					core::vector3df planeLine[4];
					for (int j = 0; j < 4; j++)
					{
						planeLine[j] = pos + (dirPlaneX * quad[j].X + dirPlaneY * quad[j].Y) * m_screenFactor;

						// save the position
						m_translsatePlane[i].Point[j] = planeLine[j];
						m_translsatePlane[i].DirX = dirPlaneX;
						m_translsatePlane[i].DirY = dirPlaneY;
					}

					const SColor& color = m_hoverOnPlane[i] ? m_selectionColor : m_directionColor[i];

					m_data->addLine(planeLine[0], planeLine[1], color);
					m_data->addLine(planeLine[1], planeLine[2], color);
					m_data->addLine(planeLine[2], planeLine[3], color);
					m_data->addLine(planeLine[3], planeLine[0], color);

					SColor fillColor = color;
					fillColor.setAlpha(50);
					m_data->addPolygonFill(planeLine, 4, fillColor);
				}
			}
		}

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		float CHandlesRenderer::getSegmentLengthClipSpace(const core::vector3df& start, const core::vector3df& end, CCamera* camera)
		{
			core::matrix4 trans = getVideoDriver()->getTransform(video::ETS_VIEW_PROJECTION);

			f32 start4[4] = { start.X, start.Y, start.Z, 1.0f };
			f32 end4[4] = { end.X, end.Y, end.Z, 1.0f };

			trans.multiplyWith1x4Matrix(start4);
			core::vector3df startOfSegment(start4[0], start4[1], start4[2]);
			if (fabsf(start4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
			{
				startOfSegment *= 1.f / start4[3];
			}

			trans.multiplyWith1x4Matrix(end4);
			core::vector3df endOfSegment(end4[0], end4[1], end4[2]);
			if (fabsf(end4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
			{
				endOfSegment *= 1.f / end4[3];
			}

			core::vector3df clipSpaceAxis = endOfSegment - startOfSegment;
			clipSpaceAxis.Y /= camera->getAspect();
			float segmentLengthInClipSpace = sqrtf(clipSpaceAxis.X * clipSpaceAxis.X + clipSpaceAxis.Y * clipSpaceAxis.Y);
			return segmentLengthInClipSpace;
		}

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		float CHandlesRenderer::getParallelogram(const core::vector3df& ptO, const core::vector3df& ptA, const core::vector3df& ptB, CCamera* camera)
		{
			core::matrix4 trans = getVideoDriver()->getTransform(video::ETS_VIEW_PROJECTION);
			core::vector3df pts[] = { ptO, ptA, ptB };

			for (unsigned int i = 0; i < 3; i++)
			{
				f32 vec4[4] = { pts[i].X, pts[i].Y, pts[i].Z, 1.0f };
				trans.multiplyWith1x4Matrix(vec4);
				pts[i].set(vec4[0], vec4[1], vec4[2]);
				if (fabsf(vec4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
				{
					pts[i] *= 1.f / vec4[3];
				}
			}

			core::vector3df segA = pts[1] - pts[0];
			core::vector3df segB = pts[2] - pts[0];
			segA.Y /= camera->getAspect();
			segB.Y /= camera->getAspect();
			core::vector3df segAOrtho = core::vector3df(-segA.Y, segA.X, 0.0f);
			segAOrtho.normalize();
			float dt = segAOrtho.dotProduct(segB);
			float surface = sqrtf(segA.X * segA.X + segA.Y * segA.Y) * fabsf(dt);
			return surface;
		}

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		void CHandlesRenderer::computeTripodAxisAndVisibility(int axisIndex, const core::vector3df& origin, core::vector3df& dirAxis, core::vector3df& dirPlaneX, core::vector3df& dirPlaneY, bool& belowAxisLimit, bool& belowPlaneLimit, CCamera* camera)
		{
			dirAxis = m_directionUnary[axisIndex];
			dirPlaneX = m_directionUnary[(axisIndex + 1) % 3];
			dirPlaneY = m_directionUnary[(axisIndex + 2) % 3];

			float lenDir = getSegmentLengthClipSpace(origin, origin + dirAxis, camera);
			float lenDirMinus = getSegmentLengthClipSpace(origin, origin - dirAxis, camera);

			float lenDirPlaneX = getSegmentLengthClipSpace(origin, origin + dirPlaneX, camera);
			float lenDirMinusPlaneX = getSegmentLengthClipSpace(origin, origin - dirPlaneX, camera);

			float lenDirPlaneY = getSegmentLengthClipSpace(origin, origin + dirPlaneY, camera);
			float lenDirMinusPlaneY = getSegmentLengthClipSpace(origin, origin - dirPlaneY, camera);

			// For readability
			bool& allowFlip = m_allowAxisFlip;
			float mulAxis = (allowFlip && lenDir < lenDirMinus&& fabsf(lenDir - lenDirMinus) > FLT_EPSILON) ? -1.f : 1.f;
			float mulAxisX = (allowFlip && lenDirPlaneX < lenDirMinusPlaneX&& fabsf(lenDirPlaneX - lenDirMinusPlaneX) > FLT_EPSILON) ? -1.f : 1.f;
			float mulAxisY = (allowFlip && lenDirPlaneY < lenDirMinusPlaneY&& fabsf(lenDirPlaneY - lenDirMinusPlaneY) > FLT_EPSILON) ? -1.f : 1.f;
			dirAxis *= mulAxis;
			dirPlaneX *= mulAxisX;
			dirPlaneY *= mulAxisY;

			// for axis
			float axisLengthInClipSpace = getSegmentLengthClipSpace(origin, origin + dirAxis * m_screenFactor, camera);
			float paraSurf = getParallelogram(origin, origin + dirPlaneX * m_screenFactor, origin + dirPlaneY * m_screenFactor, camera);

			belowPlaneLimit = (paraSurf > 0.0025f);
			belowAxisLimit = (axisLengthInClipSpace > 0.02f);

			// and store values
			m_axisFactor[axisIndex] = mulAxis;
			m_axisFactor[(axisIndex + 1) % 3] = mulAxisX;
			m_axisFactor[(axisIndex + 2) % 3] = mulAxisY;
			m_belowAxisLimit[axisIndex] = belowAxisLimit;
			m_belowPlaneLimit[axisIndex] = belowPlaneLimit;
		}

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		core::vector3df CHandlesRenderer::pointOnSegment(const core::vector3df& point, const core::vector3df& vertPos1, const core::vector3df& vertPos2)
		{
			core::vector3df c = point - vertPos1;
			core::vector3df v;

			v = vertPos2 - vertPos1;
			v.normalize();

			float d = (vertPos2 - vertPos1).getLength();
			float t = v.dotProduct(c);

			if (t < 0.f)
			{
				return vertPos1;
			}

			if (t > d)
			{
				return vertPos2;
			}

			return vertPos1 + v * t;
		}

		void CHandlesRenderer::render(CEntityManager* entityManager)
		{

		}

		void CHandlesRenderer::postRender(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			IMeshBuffer* buffer = NULL;

			buffer = m_data->PolygonBuffer;
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);

			buffer = m_data->LineBuffer;
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);
		}

		void CHandlesRenderer::onMouseEvent(int x, int y, int state)
		{
			if (m_enable == false || m_camera == NULL)
				return;

			CHandles* handles = CHandles::getInstance();
			if (handles->isHandlePosition())
				handleTranslate(x, y, state);
		}

		void CHandlesRenderer::handleTranslate(int x, int y, int state)
		{
			core::vector3df mouse((float)x, (float)y, 0.0f);

			int vpWidth = m_viewport.getWidth();
			int vpHeight = m_viewport.getHeight();

			CHandles* handle = CHandles::getInstance();

			if (m_mouseDown == false)
			{
				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
					m_hoverOnPlane[i] = false;
				}

				// check hittest axis or plane on mouse move
				for (int i = 0; i < 3; i++)
				{
					if (m_belowPlaneLimit[i] && m_screenFactor > 0.0f)
					{
						core::line3df viewRay = CProjective::getViewRay(m_camera, mouse.X, mouse.Y, vpWidth, vpHeight);
						core::vector3df n = m_translateAxis[i].getVector();
						n.normalize();
						core::plane3df plane(m_translateAxis[i].start, n);
						core::vector3df out;
						float quadMin = 0.1f;
						float quadMax = 0.4f;

						if (plane.getIntersectionWithLine(viewRay.start, viewRay.getVector(), out))
						{
							core::vector3df v = out - m_translateAxis[i].start;

							// Project vector V to dirX and dirY
							const float dx = m_translsatePlane[i].DirX.dotProduct(v / m_screenFactor);
							const float dy = m_translsatePlane[i].DirY.dotProduct(v / m_screenFactor);

							if (dx >= quadMin && dx <= quadMax && dy >= quadMin && dy < quadMax)
							{
								m_hoverOnPlane[i] = true;
								break;
							}
							else
							{
								m_hoverOnPlane[i] = false;
							}
						}
					}

					if (m_belowAxisLimit[i] && !m_hoverOnPlane[i])
					{
						core::vector3df start, end;
						if (CProjective::getScreenCoordinatesFrom3DPosition(m_camera, m_translateAxis[i].start, start.X, start.Y, vpWidth, vpHeight))
						{
							if (CProjective::getScreenCoordinatesFrom3DPosition(m_camera, m_translateAxis[i].end, end.X, end.Y, vpWidth, vpHeight))
							{
								core::vector3df projective = pointOnSegment(mouse, start, end);
								float length = projective.getDistanceFrom(mouse);
								float length1 = mouse.getDistanceFrom(start);

								if (length < 10.0f && length1 > 10.0f)
								{
									m_hoverOnAxis[i] = true;
									break;
								}
								else
								{
									m_hoverOnAxis[i] = false;
								}
							}
						}
					}
				}
			}
			else
			{
				// mouse dragging
				for (int i = 0; i < 3; i++)
				{
					if (m_hoverOnAxis[i])
					{
						core::vector3df vec = m_translsatePlane[i].DirX;
						vec.normalize();

						core::line3df viewRay0 = CProjective::getViewRay(m_camera, m_lastMouse.X, m_lastMouse.Y, vpWidth, vpHeight);
						core::line3df viewRay1 = CProjective::getViewRay(m_camera, mouse.X, mouse.Y, vpWidth, vpHeight);

						core::plane3df plane(m_translateAxis[i].start, vec);
						core::vector3df out0, out1;

						if (plane.getIntersectionWithLine(viewRay0.start, viewRay0.getVector(), out0))
						{
							if (plane.getIntersectionWithLine(viewRay1.start, viewRay1.getVector(), out1))
							{
								core::vector3df axis = m_translateAxis[i].getVector();
								axis.normalize();

								float h0 = axis.dotProduct(out0);
								float h1 = axis.dotProduct(out1);

								core::vector3df offset = axis * (h1 - h0);
								core::vector3df result = m_lastTranslatePosition + offset;
								handle->setTranslateOffset(result - handle->getHandlePosition());

								break;
							}
						}
					}

					if (m_hoverOnPlane[i])
					{
						core::vector3df vec = m_translateAxis[i].getVector();
						vec.normalize();

						core::line3df viewRay0 = CProjective::getViewRay(m_camera, m_lastMouse.X, m_lastMouse.Y, vpWidth, vpHeight);
						core::line3df viewRay1 = CProjective::getViewRay(m_camera, mouse.X, mouse.Y, vpWidth, vpHeight);

						core::plane3df plane(m_translateAxis[i].start, vec);
						core::vector3df out0, out1;

						if (plane.getIntersectionWithLine(viewRay0.start, viewRay0.getVector(), out0))
						{
							if (plane.getIntersectionWithLine(viewRay1.start, viewRay1.getVector(), out1))
							{
								core::vector3df offset = out1 - out0;
								core::vector3df result = m_lastTranslatePosition + offset;
								handle->setTranslateOffset(result - handle->getHandlePosition());

								break;
							}
						}
					}
				}
			}

			if (m_mouseState != state)
			{
				if (state == 1)
				{
					// mouse down
					m_mouseDown = true;
					m_lastMouse = mouse;
					m_lastTranslatePosition = handle->getHandlePosition();
				}
				else if (state == 2)
				{
					// mouse up
					m_mouseDown = false;
				}
			}

			m_mouseState = state;
		}
	}
}