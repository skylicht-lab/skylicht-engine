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
#include "CHandlesRenderer.h"
#include "Handles/CHandles.h"
#include "Entity/CEntityManager.h"
#include "Projective/CProjective.h"
#include "Debug/CSceneDebug.h"

namespace Skylicht
{
	namespace Editor
	{
		CHandlesRenderer::CHandlesRenderer() :
			m_screenFactor(1.0f),
			m_allowAxisFlip(true),
			m_camera(NULL),
			m_mouseDown(false),
			m_mouseState(0),
			m_cancel(false),
			m_using(false),
			m_usingEvent(false),
			m_rotationAngle(0.0f),
			m_enable(true),
			m_hoverOnPointId(-1),
			m_haveDrawed(0)
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
			m_directionColor[1].set(0xFF00AA00);
			m_directionColor[2].set(0xFFAA0000);

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

		void CHandlesRenderer::beginQuery(CEntityManager* entityManager)
		{

		}

		void CHandlesRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{

		}

		void CHandlesRenderer::init(CEntityManager* entityManager)
		{

		}

		void CHandlesRenderer::update(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			if (entityManager->getRenderPipeline()->getType() != IRenderPipeline::Forwarder)
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

			CHandles* handles = CHandles::getInstance();

			// Caculate the screen factor
			core::vector3df pos = handles->getHandlePosition();
			handles->getWorld().transformVect(pos);

			core::quaternion rot = handles->getHandleRotation() * getRotation(handles->getWorld());

			m_screenFactor = 0.2f / CProjective::getSegmentLengthClipSpace(camera, pos, pos + cameraRight);
			m_haveDrawed = 0;
			
			if (handles->isHandlePosition())
			{
				drawTranslateGizmo(pos, rot, cameraPos, camera);
				m_haveDrawed = 1;
			}
			else if (handles->isHandleRotation())
			{
				drawRotationGizmo(pos, cameraPos, rot);
				m_haveDrawed = 2;
			}
			else if (handles->isHandleScale())
			{
				drawScaleGizmo(pos, rot, cameraLook, cameraUp, camera);
				m_haveDrawed = 3;
			}
			else if (handles->isHandleListPosition())
			{
				drawListPositionGizmo(
					handles->getListPosition(), handles->getSelectId(),
					rot,
					cameraPos, cameraRight, camera
				);
				m_haveDrawed = 4;
			}

			((CLineDrawData*)m_data)->updateBuffer();
			((CPolygonDrawData*)m_data)->updateBuffer();
		}

		void CHandlesRenderer::drawRotationGizmo(const core::vector3df& pos, const core::vector3df& cameraPos, const core::quaternion& rot)
		{
			core::vector3df viewVector = pos - cameraPos;
			viewVector.normalize();

			const int halfCircleSegmentCount = 64;
			const int circleMul = 2;

			core::vector3df circlePos[circleMul * halfCircleSegmentCount + 1];

			core::vector3df normal[] = {
				{0.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 0.0f},
				{1.0f, 0.0f, 0.0f}
			};

			core::quaternion localRotation = rot;

			if (CHandles::getInstance()->useLocalSpace())
			{
				for (int i = 0; i < 3; i++)
				{
					normal[i] = rot * normal[i];
					normal[i].normalize();
				}
			}
			else
			{
				localRotation.makeIdentity();
			}

			for (int axis = 0; axis < 3; axis++)
			{
				video::SColor color = m_hoverOnAxis[axis] ? m_selectionColor : m_directionColor[axis];
				core::vector3df lastPosition;

				// draw rotation
				for (unsigned int i = 0; i <= circleMul * halfCircleSegmentCount; i++)
				{
					float ng = core::PI * ((float)i / (float)halfCircleSegmentCount);

					core::vector3df axisPos = core::vector3df(cosf(ng), sinf(ng), 0.f);
					float* p = &axisPos.X;

					// circle point
					core::vector3df r = core::vector3df(p[axis], p[(axis + 1) % 3], p[(axis + 2) % 3]);

					// local rotation
					r = localRotation * r;
					r.normalize();

					SColor lineColor = color;

					// back face
					if (viewVector.dotProduct(r) > FLT_EPSILON)
					{
						lineColor.setAlpha(50);
					}

					core::vector3df circlePosition = pos + r * m_screenFactor;

					if (i > 0)
					{
						m_data->addLine(lastPosition, circlePosition, lineColor);
					}

					lastPosition = circlePosition;
				}
			}

			if (m_using)
			{
				int hitAxis = 0;
				for (int i = 0; i < 3; i++)
				{
					if (m_hoverOnAxis[i] == true)
					{
						hitAxis = i;
						break;
					}
				}

				SColor fillColor = m_selectionColor;
				fillColor.setAlpha(50);

				if (fabsf(m_rotationAngle) > FLT_EPSILON)
				{
					circlePos[0] = pos;
					for (int i = 1; i < halfCircleSegmentCount; i++)
					{
						float ng = m_rotationAngle * ((float)(i - 1) / (float)(halfCircleSegmentCount - 1));
						core::quaternion rotateVector;
						rotateVector.fromAngleAxis(ng, normal[hitAxis]);

						core::vector3df arcPos;
						arcPos = rotateVector * m_rotationVectorSource;
						arcPos *= m_screenFactor;

						circlePos[i] = pos + arcPos;
					}

					m_data->addPolygonFill(circlePos, halfCircleSegmentCount, fillColor);
					m_data->addPolyline(circlePos, halfCircleSegmentCount, true, m_selectionColor);
				}
				else
				{
					m_data->addLine(pos, pos + m_rotationVectorSource * m_screenFactor, m_selectionColor);
				}
			}
		}

		void CHandlesRenderer::drawScaleGizmo(const core::vector3df& pos, const core::quaternion& rot, const core::vector3df& cameraLook, const core::vector3df& cameraUp, CCamera* camera)
		{
			CHandles* handles = CHandles::getInstance();

			float planeMin = 0.3f;

			for (int i = 0; i < 3; i++)
			{
				core::vector3df dirAxis, dirPlaneX, dirPlaneY;
				bool belowAxisLimit, belowPlaneLimit;

				computeTripodAxisAndVisibility(i, pos, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit, camera, rot);

				m_scaleAxis[i].start = pos;
				m_scaleAxis[i].end = pos + dirAxis * m_screenFactor;

				if (belowAxisLimit)
				{
					// update draging axis with scale
					core::vector3df scale(1.0f, 1.0f, 1.0f);
					if (m_using)
						scale = handles->getHandleScale() / m_lastScale;
					float* s = &scale.X;

					// apply drag scale
					core::vector3df end = pos + dirAxis * m_screenFactor * s[i];

					// draw axis
					m_data->addLine(m_scaleAxis[i].start, end, m_hoverOnAxis[i] ? m_selectionColor : m_directionColor[i]);

					// plane
					m_scalePlane.Point[i] = pos + dirAxis * m_screenFactor * planeMin;

					// draw quad
					float size = m_screenFactor * 0.05f;

					core::vector3df side = cameraUp.crossProduct(cameraLook);
					side.normalize();

					core::vector3df sideQuad = side * size;
					core::vector3df upQuad = cameraUp * size;

					core::vector3df v1, v2, v3, v4;
					v1.set(
						end.X - sideQuad.X + upQuad.X,
						end.Y - sideQuad.Y + upQuad.Y,
						end.Z - sideQuad.Z + upQuad.Z
					);
					v2.set(
						end.X + sideQuad.X + upQuad.X,
						end.Y + sideQuad.Y + upQuad.Y,
						end.Z + sideQuad.Z + upQuad.Z
					);
					v3.set(
						end.X + sideQuad.X - upQuad.X,
						end.Y + sideQuad.Y - upQuad.Y,
						end.Z + sideQuad.Z - upQuad.Z
					);
					v4.set(
						end.X - sideQuad.X - upQuad.X,
						end.Y - sideQuad.Y - upQuad.Y,
						end.Z - sideQuad.Z - upQuad.Z
					);
					core::vector3df quad[] = { v1, v2, v3, v4 };
					m_data->addPolygonFill(quad, 4, m_hoverOnAxis[i] ? m_selectionColor : m_directionColor[i]);

					// save plan vector
					m_translatePlane[i].DirX = dirPlaneX;
					m_translatePlane[i].DirY = dirPlaneY;
				}
			}

			// draw scale plane
			video::SColor color = m_selectionColor;
			color.setAlpha(50);
			m_data->addPolygonFill(m_scalePlane.Point, 3, m_hoverOnPlane[0] ? m_selectionColor : color);
			m_data->addPolyline(m_scalePlane.Point, 3, true, m_selectionColor);
		}

		void CHandlesRenderer::drawTranslateGizmo(const core::vector3df& pos, const core::quaternion& rot, const core::vector3df& cameraPos, CCamera* camera)
		{
			float quadMin = 0.1f;
			float quadMax = 0.4f;

			core::vector3df quad[4] = {
				core::vector3df(quadMin, quadMin, 0.0f),
				core::vector3df(quadMin, quadMax, 0.0f),
				core::vector3df(quadMax, quadMax, 0.0f),
				core::vector3df(quadMax, quadMin, 0.0f),
			};

			core::quaternion localRotation = rot;
			if (!CHandles::getInstance()->useLocalSpace())
				localRotation.makeIdentity();

			for (int i = 0; i < 3; i++)
			{
				core::vector3df dirAxis, dirPlaneX, dirPlaneY;
				bool belowAxisLimit, belowPlaneLimit;

				computeTripodAxisAndVisibility(i, pos, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit, camera, localRotation);

				// save the position
				m_translateAxis[i].start = pos;
				m_translateAxis[i].end = pos + dirAxis * m_screenFactor;

				if (belowAxisLimit)
				{
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

				core::vector3df planeLine[4];
				for (int j = 0; j < 4; j++)
				{
					planeLine[j] = pos + (dirPlaneX * quad[j].X + dirPlaneY * quad[j].Y) * m_screenFactor;

					// save the position
					m_translatePlane[i].Point[j] = planeLine[j];
					m_translatePlane[i].DirX = dirPlaneX;
					m_translatePlane[i].DirY = dirPlaneY;
				}

				// draw plane
				if (belowPlaneLimit)
				{
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

			if (m_using)
			{
				core::vector3df lastPosition = m_lastTranslatePosition;

				video::SColor color(255, 100, 100, 100);
				m_data->addLine(lastPosition, pos, color);

				core::vector3df d = pos - lastPosition;
				core::vector3df p;
				float min = 0.001f;

				if (fabsf(d.X) > min)
				{
					color.set(100, 50, 0, 0);
					p = lastPosition;
					p.X += d.X;
					m_data->addLine(lastPosition, p, color);
					m_data->addLine(pos, p, color);
				}

				if (fabsf(d.Y) > min)
				{
					color.set(100, 0, 50, 0);
					p = lastPosition;
					p.Y += d.Y;
					m_data->addLine(lastPosition, p, color);
					m_data->addLine(pos, p, color);
				}

				if (fabsf(d.Z) > min)
				{
					color.set(100, 0, 0, 50);
					p = lastPosition;
					p.Z += d.Z;
					m_data->addLine(lastPosition, p, color);
					m_data->addLine(pos, p, color);
				}
			}
		}

		void CHandlesRenderer::drawListPositionGizmo(std::vector<core::vector3df>& pos, int selectId, const core::quaternion& rot, const core::vector3df& cameraPos, const core::vector3df& cameraRight, CCamera* camera)
		{
			CHandles* handles = CHandles::getInstance();

			int pointId = 0;

			for (core::vector3df& p : pos)
			{
				float size = 0.01f / CProjective::getSegmentLengthClipSpace(camera, p, p + cameraRight);

				if (pointId == m_hoverOnPointId)
					handles->drawRectBillboard(p, size, size, m_selectionColor);
				else
					handles->drawRectBillboard(p, size, size, m_directionColor[1]);

				pointId++;
			}

			drawTranslateGizmo(pos[selectId], rot, cameraPos, camera);
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
		void CHandlesRenderer::computeTripodAxisAndVisibility(int axisIndex, const core::vector3df& origin, core::vector3df& dirAxis, core::vector3df& dirPlaneX, core::vector3df& dirPlaneY, bool& belowAxisLimit, bool& belowPlaneLimit, CCamera* camera, const core::quaternion& rot)
		{
			dirAxis = m_directionUnary[axisIndex];
			dirPlaneX = m_directionUnary[(axisIndex + 1) % 3];
			dirPlaneY = m_directionUnary[(axisIndex + 2) % 3];

			dirAxis = rot * dirAxis;
			dirAxis.normalize();

			dirPlaneX = rot * dirPlaneX;
			dirPlaneX.normalize();

			dirPlaneY = rot * dirPlaneY;
			dirPlaneY.normalize();

			if (m_using)
			{
				belowAxisLimit = m_belowAxisLimit[axisIndex];
				belowPlaneLimit = m_belowPlaneLimit[axisIndex];

				dirAxis *= m_axisFactor[axisIndex];
				dirPlaneX *= m_axisFactor[(axisIndex + 1) % 3];
				dirPlaneY *= m_axisFactor[(axisIndex + 2) % 3];
			}
			else
			{
				float lenDir = CProjective::getSegmentLengthClipSpace(camera, origin, origin + dirAxis);
				float lenDirMinus = CProjective::getSegmentLengthClipSpace(camera, origin, origin - dirAxis);

				float lenDirPlaneX = CProjective::getSegmentLengthClipSpace(camera, origin, origin + dirPlaneX);
				float lenDirMinusPlaneX = CProjective::getSegmentLengthClipSpace(camera, origin, origin - dirPlaneX);

				float lenDirPlaneY = CProjective::getSegmentLengthClipSpace(camera, origin, origin + dirPlaneY);
				float lenDirMinusPlaneY = CProjective::getSegmentLengthClipSpace(camera, origin, origin - dirPlaneY);

				// For readability
				bool allowFlip = m_allowAxisFlip && camera->getProjectionType() == CCamera::Perspective;
				const float epsilon = 0.001f;

				float mulAxis = (allowFlip && lenDir < lenDirMinus && fabsf(lenDir - lenDirMinus) > epsilon) ? -1.f : 1.f;
				float mulAxisX = (allowFlip && lenDirPlaneX < lenDirMinusPlaneX && fabsf(lenDirPlaneX - lenDirMinusPlaneX) > epsilon) ? -1.f : 1.f;
				float mulAxisY = (allowFlip && lenDirPlaneY < lenDirMinusPlaneY && fabsf(lenDirPlaneY - lenDirMinusPlaneY) > epsilon) ? -1.f : 1.f;

				dirAxis *= mulAxis;
				dirPlaneX *= mulAxisX;
				dirPlaneY *= mulAxisY;

				// for axis
				float axisLengthInClipSpace = CProjective::getSegmentLengthClipSpace(camera, origin, origin + dirAxis * m_screenFactor);
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

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		float CHandlesRenderer::computeAngleOnPlan(const core::vector3df& vector, const core::vector3df& normal)
		{
			core::vector3df perpendicularVector;
			perpendicularVector = m_rotationVectorSource.crossProduct(normal);
			perpendicularVector.normalize();

			float acosAngle = core::clamp(vector.dotProduct(m_rotationVectorSource), -1.f, 1.f);
			float angle = acosf(acosAngle);
			angle *= (vector.dotProduct(perpendicularVector) < 0.f) ? 1.f : -1.f;
			return angle;
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

		float CHandlesRenderer::snap(float value, float d)
		{
			if (d == 0.0f)
				return value;

			float f = value / d;

			float f1 = floorf(f);
			float f2 = ceilf(f);

			if (fabsf(f1 - f) < fabsf(f2 - f))
				return f1 * d;

			return f2 * d;
		}

		void CHandlesRenderer::snapVec3(core::vector3df& v)
		{
			CHandles* handles = CHandles::getInstance();
			if (handles->isSnapXZ())
			{
				float snapDistance = handles->getSnapDistanceXZ();
				v.X = snap(v.X, snapDistance);
				v.Z = snap(v.Z, snapDistance);
			}
			if (handles->isSnapY())
			{
				float snapDistance = handles->getSnapDistanceY();
				v.Y = snap(v.Y, snapDistance);
			}
		}

		void CHandlesRenderer::onMouseEvent(int x, int y, int state)
		{
			if (m_enable == false || m_camera == NULL || m_haveDrawed == 0)
				return;

			CHandles* handles = CHandles::getInstance();
			if (handles->isHandlePosition() && m_haveDrawed == 1)
				handleTranslate(x, y, state);
			else if (handles->isHandleRotation() && m_haveDrawed == 2)
				handleRotation(x, y, state);
			else if (handles->isHandleScale() && m_haveDrawed == 3)
				handleScale(x, y, state);
			else if (handles->isHandleListPosition() && m_haveDrawed == 4)
			{
				handleTranslate(x, y, state);
				handleSelectPoint(x, y, state);
			}
		}

		void CHandlesRenderer::handleSelectPoint(int x, int y, int state)
		{
			bool hoverOnAxisOrPlane = false;
			for (int i = 0; i < 3; i++)
			{
				if (m_hoverOnAxis[i] || m_hoverOnPlane[i])
				{
					hoverOnAxisOrPlane = true;
					break;
				}
			}

			if (!hoverOnAxisOrPlane)
			{
				CHandles* handles = CHandles::getInstance();

				core::vector3df mouse((float)x, (float)y, 0.0f);

				if (m_mouseDown == false)
				{
					m_hoverOnPointId = -1;

					int vpWidth = m_viewport.getWidth();
					int vpHeight = m_viewport.getHeight();

					core::vector3df look = m_camera->getLookVector();
					core::vector3df up = m_camera->getUpVector();
					core::vector3df right = up.crossProduct(look);
					right.normalize();

					std::vector<core::vector3df>& points = handles->getListPosition();
					int pointId = 0;

					for (core::vector3df& p : points)
					{
						core::vector3df projectPoint;
						float size = 0.5f * (0.01f / CProjective::getSegmentLengthClipSpace(m_camera, p, p + right));
						if (CProjective::getScreenCoordinatesFrom3DPosition(m_camera, p, projectPoint.X, projectPoint.Y, vpWidth, vpHeight))
						{
							if (mouse.getDistanceFromSQ(projectPoint) < size * size)
							{
								m_hoverOnPointId = pointId;
								break;
							}
						}

						pointId++;
					}
				}
				else
				{
					if (m_hoverOnPointId != -1)
					{
						handles->setSelectId(m_hoverOnPointId);
					}
				}
			}
		}

		void CHandlesRenderer::handleTranslate(int x, int y, int state)
		{
			core::vector3df mouse((float)x, (float)y, 0.0f);

			int vpWidth = m_viewport.getWidth();
			int vpHeight = m_viewport.getHeight();

			CHandles* handles = CHandles::getInstance();

			if (m_mouseState != state)
			{
				if (state == 1)
				{
					// mouse down
					m_usingEvent = false;
					m_mouseDown = true;
					m_lastMouse = mouse;
					m_lastTranslatePosition = handles->getHandlePosition();
					handles->getWorld().transformVect(m_lastTranslatePosition);
				}
				else if (state == 2)
				{
					// mouse up
					if (!m_cancel)
					{
						m_mouseDown = false;
						m_using = false;
						handles->setEndCheck(true);
					}
					m_cancel = false;
				}
			}

			m_mouseState = state;

			core::vector3df resultPosition;

			if (!m_cancel)
			{
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
								const float dx = m_translatePlane[i].DirX.dotProduct(v / m_screenFactor);
								const float dy = m_translatePlane[i].DirY.dotProduct(v / m_screenFactor);

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
							if (m_using == false)
							{
								const core::vector3df& v1 = m_translatePlane[i].DirX;
								const core::vector3df& v2 = m_translatePlane[i].DirY;

								core::vector3df look = m_camera->getLookVector();
								float d1 = look.dotProduct(v1);
								float d2 = look.dotProduct(v2);

								if (d1 < d2)
									m_planNormal = v1;
								else
									m_planNormal = v2;
							}

							m_using = true;
							m_usingEvent = true;

							core::vector3df vec = m_planNormal;
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
									resultPosition = m_lastTranslatePosition + offset;

									handles->getWorldInv().transformVect(resultPosition);
									snapVec3(resultPosition);
									handles->setTargetPosition(resultPosition);
									break;
								}
							}
						}

						if (m_hoverOnPlane[i])
						{
							m_using = true;
							m_usingEvent = true;

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
									resultPosition = m_lastTranslatePosition + offset;

									handles->getWorldInv().transformVect(resultPosition);
									snapVec3(resultPosition);
									handles->setTargetPosition(resultPosition);

									break;
								}
							}
						}
					}
				} // mouse down
			} // cancel
		}

		void CHandlesRenderer::handleRotation(int x, int y, int state)
		{
			core::vector3df mouse((float)x, (float)y, 0.0f);

			int vpWidth = m_viewport.getWidth();
			int vpHeight = m_viewport.getHeight();

			CHandles* handles = CHandles::getInstance();

			core::vector3df pos = handles->getHandlePosition();
			handles->getWorld().transformVect(pos);

			core::quaternion rot = handles->getRotationOrigin() * getRotation(handles->getWorld());

			if (m_mouseState != state)
			{
				if (state == 1)
				{
					// mouse down
					m_mouseDown = true;
					m_usingEvent = false;
					m_lastMouse = mouse;

					m_lastRotation = handles->getHandleRotation();
				}
				else if (state == 2)
				{
					// mouse up
					if (!m_cancel)
					{
						m_mouseDown = false;
						m_using = false;
						handles->setEndCheck(true);
					}
					m_cancel = false;
				}
			}

			m_mouseState = state;

			core::line3df viewRay = CProjective::getViewRay(m_camera, mouse.X, mouse.Y, vpWidth, vpHeight);

			core::vector3df worldNormal[] = {
				{0.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 0.0f},
				{1.0f, 0.0f, 0.0f}
			};

			core::vector3df localNormal[] = {
				{0.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 0.0f},
				{1.0f, 0.0f, 0.0f}
			};

			if (handles->useLocalSpace())
			{
				for (int i = 0; i < 3; i++)
				{
					worldNormal[i] = rot * worldNormal[i];
					worldNormal[i].normalize();

					localNormal[i] = handles->getRotationOrigin() * localNormal[i];
					localNormal[i].normalize();
				}
			}

			core::vector3df out;
			core::quaternion resultRotation;

			core::vector3df viewVector = pos - m_camera->getPosition();
			viewVector.normalize();

			if (!m_cancel)
			{
				if (m_mouseDown == false)
				{
					for (int i = 0; i < 3; i++)
						m_hoverOnAxis[i] = false;

					for (int i = 0; i < 3; i++)
					{
						core::plane3df p(pos, worldNormal[i]);

						core::vector3df viewVector = viewRay.getVector();
						viewVector.normalize();

						if (p.getIntersectionWithLine(viewRay.start, viewVector, out))
						{
							core::vector3df hitVector = out - pos;
							hitVector.normalize();

							if (viewVector.dotProduct(hitVector) > FLT_EPSILON)
							{
								// if hover in back circle
								continue;
							}

							m_rotationVectorSource = hitVector;
							m_rotationAngle = 0.0f;

							core::vector3df idealPosOnCircle = out - pos;
							idealPosOnCircle.normalize();
							idealPosOnCircle *= m_screenFactor;
							idealPosOnCircle += pos;

							core::vector3df p1, p2;
							if (CProjective::getScreenCoordinatesFrom3DPosition(m_camera, out, p1.X, p1.Y, vpWidth, vpHeight))
							{
								if (CProjective::getScreenCoordinatesFrom3DPosition(m_camera, idealPosOnCircle, p2.X, p2.Y, vpWidth, vpHeight))
								{
									float length = p1.getDistanceFrom(p2);
									if (length < 10.0f)
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
					for (int i = 0; i < 3; i++)
					{
						if (m_hoverOnAxis[i])
						{
							m_using = true;
							m_usingEvent = true;

							core::plane3df p(pos, worldNormal[i]);

							core::vector3df viewVector = viewRay.getVector();
							viewVector.normalize();

							if (p.getIntersectionWithLine(viewRay.start, viewVector, out))
							{
								core::vector3df hitVector = out - pos;
								hitVector.normalize();

								m_rotationAngle = computeAngleOnPlan(hitVector, worldNormal[i]);

								if (handles->isSnapRotate())
									m_rotationAngle = snap(m_rotationAngle, handles->getSnapRotateDeg() * core::DEGTORAD);

								core::quaternion q;
								q.fromAngleAxis(m_rotationAngle, localNormal[i]);

								resultRotation = m_lastRotation * q;
								resultRotation.normalize();

								handles->setTargetRotation(resultRotation);
							}
							break;
						}
					}
				} // mouse down
			} // cancel			
		}

		void CHandlesRenderer::handleScale(int x, int y, int state)
		{
			core::vector3df mouse((float)x, (float)y, 0.0f);

			int vpWidth = m_viewport.getWidth();
			int vpHeight = m_viewport.getHeight();

			CHandles* handles = CHandles::getInstance();

			if (m_mouseState != state)
			{
				if (state == 1)
				{
					// mouse down
					const core::vector3df& s = handles->getHandleScale();
					if (s.X != 0 && s.Y != 0 && s.Z != 0)
					{
						m_mouseDown = true;
						m_usingEvent = false;
						m_lastMouse = mouse;
						m_lastScale = handles->getHandleScale();
					}
				}
				else if (state == 2)
				{
					// mouse up
					if (!m_cancel)
					{
						m_mouseDown = false;
						m_using = false;
						handles->setEndCheck(true);
					}
					m_cancel = false;
				}
			}

			m_mouseState = state;

			core::vector3df resultScale;

			if (!m_cancel)
			{
				if (m_mouseDown == false)
				{
					for (int i = 0; i < 3; i++)
						m_hoverOnAxis[i] = false;

					m_hoverOnPlane[0] = false;

					// check hittest axis or plane on mouse move
					core::line3df viewRay = CProjective::getViewRay(m_camera, mouse.X, mouse.Y, vpWidth, vpHeight);
					core::vector3df out;

					core::triangle3df tris(m_scalePlane.Point[0], m_scalePlane.Point[1], m_scalePlane.Point[2]);
					if (tris.getIntersectionWithLine(viewRay.start, viewRay.getVector(), out))
					{
						if (tris.isPointInside(out))
						{
							m_hoverOnPlane[0] = true;
						}
					}

					if (m_hoverOnPlane[0] == false)
					{
						for (int i = 0; i < 3; i++)
						{
							if (m_belowAxisLimit[i])
							{
								core::vector3df start, end;
								if (CProjective::getScreenCoordinatesFrom3DPosition(m_camera, m_scaleAxis[i].start, start.X, start.Y, vpWidth, vpHeight))
								{
									if (CProjective::getScreenCoordinatesFrom3DPosition(m_camera, m_scaleAxis[i].end, end.X, end.Y, vpWidth, vpHeight))
									{
										core::vector3df projective = pointOnSegment(mouse, start, end);
										float length = projective.getDistanceFrom(mouse);
										float length1 = mouse.getDistanceFrom(start);

										if (length < 10.0f)
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
				}
				else
				{
					// mouse dragging
					if (m_hoverOnPlane[0] == true)
					{
						float dx = mouse.X - m_lastMouse.X;
						float dy = m_lastMouse.Y - mouse.Y;
						
						float d = core::max_(vpWidth, vpHeight) * 0.2f;

						float drag = dx + dy;

						if (d > 0.0f)
						{
							m_using = true;
							m_usingEvent = true;

							float scale = 1.0f + drag / d;
							resultScale = m_lastScale * scale;
							handles->setTargetScale(resultScale);
						}
					}
					else
					{
						for (int i = 0; i < 3; i++)
						{
							if (m_hoverOnAxis[i])
							{
								if (m_using == false)
								{
									const core::vector3df& v1 = m_translatePlane[i].DirX;
									const core::vector3df& v2 = m_translatePlane[i].DirY;

									core::vector3df look = m_camera->getLookVector();
									float d1 = look.dotProduct(v1);
									float d2 = look.dotProduct(v2);

									if (d1 < d2)
										m_planNormal = v1;
									else
										m_planNormal = v2;
								}

								m_using = true;
								m_usingEvent = true;

								core::vector3df vec = m_planNormal;
								vec.normalize();

								core::line3df viewRay0 = CProjective::getViewRay(m_camera, m_lastMouse.X, m_lastMouse.Y, vpWidth, vpHeight);
								core::line3df viewRay1 = CProjective::getViewRay(m_camera, mouse.X, mouse.Y, vpWidth, vpHeight);

								core::plane3df plane(m_scaleAxis[i].start, vec);
								core::vector3df out0, out1;

								if (plane.getIntersectionWithLine(viewRay0.start, viewRay0.getVector(), out0))
								{
									if (plane.getIntersectionWithLine(viewRay1.start, viewRay1.getVector(), out1))
									{
										core::vector3df axis = m_scaleAxis[i].getVector();
										axis.normalize();

										float h0 = axis.dotProduct(out0);
										float h1 = axis.dotProduct(out1);

										core::vector3df offset = axis * (h1 - h0);

										core::vector3df from = axis * m_screenFactor;
										core::vector3df target = from + offset;

										float s[3]{ 1.0f, 1.0f, 1.0f };
										float axisLength = from.getLength();
										
										if (fabsf(axisLength) > FLT_EPSILON)
										{
											s[i] = target.getLength() / axisLength;
											
											float h = axis.dotProduct(offset);
											if (h + axisLength < 0.0f)
												s[i] = -s[i];
											
											resultScale.set(s[0], s[1], s[2]);
											resultScale *= m_lastScale;
											
											handles->setTargetScale(resultScale);
										}
										break;
									}
								}
							}
						}
					}
				} // mouse down
			} // cancel
		}

		void CHandlesRenderer::cancel()
		{
			m_mouseDown = false;
			m_using = false;
			m_usingEvent = false;

			CHandles* handles = CHandles::getInstance();
			if (handles->isHandlePosition())
			{
				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
					m_hoverOnPlane[i] = false;
				}

				core::vector3df pos = m_lastTranslatePosition;
				handles->getWorldInv().transformVect(pos);
				handles->setTargetPosition(pos);
			}
			else if (handles->isHandleRotation())
			{
				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
				}

				core::quaternion q(handles->getWorld());
				q.makeInverse();
				q.normalize();
				handles->setTargetRotation(m_lastRotation);
			}
			else if (handles->isHandleScale())
			{
				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
					m_hoverOnPlane[i] = false;
				}
				handles->setTargetScale(m_lastScale);
			}
		}

		void CHandlesRenderer::skip()
		{
			if (m_using)
			{
				m_mouseState = 0;
				m_mouseDown = false;
				m_using = false;

				for (int i = 0; i < 3; i++)
				{
					m_hoverOnAxis[i] = false;
					m_hoverOnPlane[i] = false;
				}

				m_hoverOnPointId = -1;

				CHandles* handles = CHandles::getInstance();
				handles->end();
				handles->setEndCheck(true);
			}
		}

		core::quaternion CHandlesRenderer::getRotation(const core::matrix4& mat)
		{
			core::vector3df front(0.0f, 0.0f, 1.0f);
			core::vector3df up(0.0f, 1.0f, 0.0f);

			mat.rotateVect(front);
			mat.rotateVect(up);

			front.normalize();
			up.normalize();

			core::vector3df right = up.crossProduct(front);
			right.normalize();

			core::matrix4 rotationMatrix;
			f32* matData = rotationMatrix.pointer();
			matData[0] = right.X;
			matData[1] = right.Y;
			matData[2] = right.Z;
			matData[3] = 0.0f;

			matData[4] = up.X;
			matData[5] = up.Y;
			matData[6] = up.Z;
			matData[7] = 0.0f;

			matData[8] = front.X;
			matData[9] = front.Y;
			matData[10] = front.Z;
			matData[11] = 0.0f;

			matData[12] = 0.0f;
			matData[13] = 0.0f;
			matData[14] = 0.0f;
			matData[15] = 1.0f;

			core::quaternion q(rotationMatrix);
			q.normalize();
			return q;
		}
	}
}
