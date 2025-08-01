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

#pragma once

#include "Entity/IRenderSystem.h"
#include "CHandlesData.h"

namespace Skylicht
{
	namespace Editor
	{
		class CHandlesRenderer : public IRenderSystem
		{
		public:
			struct SPlane
			{
				core::vector3df Point[4];
				core::vector3df DirX;
				core::vector3df DirY;
			};

		protected:
			CHandlesData* m_data;

			bool m_enable;
			bool m_using;
			bool m_usingEvent;

			core::vector3df m_directionUnary[3];
			video::SColor m_directionColor[3];
			video::SColor m_selectionColor;

			core::vector3df m_planNormal;

			float m_screenFactor;
			float m_axisFactor[3];
			bool m_belowAxisLimit[3];
			bool m_belowPlaneLimit[3];

			bool m_allowAxisFlip;

			bool m_hoverOnAxis[3];
			bool m_hoverOnPlane[3];

			int m_hoverOnPointId;

			core::line3df m_translateAxis[3];
			SPlane m_translatePlane[3];
			core::line3df m_scaleAxis[3];
			SPlane m_scalePlane;

			CCamera* m_camera;
			core::recti m_viewport;

			int m_mouseState;
			bool m_mouseDown;
			bool m_cancel;

			core::vector3df m_lastMouse;
			core::vector3df m_lastTranslatePosition;
			core::vector3df m_lastScale;
			core::quaternion m_lastRotation;
			core::vector3df m_rotationVectorSource;
			float m_rotationAngle;

			int m_haveDrawed;
		public:
			CHandlesRenderer();

			virtual ~CHandlesRenderer();

			virtual void beginQuery(CEntityManager* entityManager);

			virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

			virtual void init(CEntityManager* entityManager);

			virtual void update(CEntityManager* entityManager);

			virtual void render(CEntityManager* entityManager);

			virtual void postRender(CEntityManager* entityManager);

			inline void setEnable(bool b)
			{
				m_enable = b;
			}

			inline bool isEnable()
			{
				return m_enable;
			}

			bool isHoverOnAxisOrPlane()
			{
				for (int i = 0; i < 3; i++)
				{
					if (m_hoverOnAxis[i] || m_hoverOnPlane[i])
						return true;
				}

				if (m_hoverOnPointId != -1)
					return true;

				return false;
			}

			inline bool isUsing()
			{
				return m_usingEvent;
			}

		public:

			void onMouseEvent(int x, int y, int state);

			void setCameraAndViewport(CCamera* cam, const core::recti& vp)
			{
				m_camera = cam;
				m_viewport = vp;
			}

			void cancel();

			void skip();

		protected:

			void drawRotationGizmo(const core::vector3df& pos, const core::vector3df& cameraPos, const core::quaternion& rot);

			void drawScaleGizmo(const core::vector3df& pos, const core::quaternion& rot, const core::vector3df& cameraLook, const core::vector3df& cameraUp, CCamera* camera);

			void drawTranslateGizmo(const core::vector3df& pos, const core::quaternion& rot, const core::vector3df& cameraPos, CCamera* camera);

			void drawListPositionGizmo(std::vector<core::vector3df>& pos, int selectId, const core::quaternion& rot, const core::vector3df& cameraPos, const core::vector3df& cameraRight, CCamera* camera);

			float getParallelogram(const core::vector3df& ptO, const core::vector3df& ptA, const core::vector3df& ptB, CCamera* camera);

			void computeTripodAxisAndVisibility(int axisIndex, const core::vector3df& origin, core::vector3df& dirAxis, core::vector3df& dirPlaneX, core::vector3df& dirPlaneY, bool& belowAxisLimit, bool& belowPlaneLimit, CCamera* camera, const core::quaternion& rot);

			float computeAngleOnPlan(const core::vector3df& vector, const core::vector3df& normal);

			core::vector3df pointOnSegment(const core::vector3df& point, const core::vector3df& vertPos1, const core::vector3df& vertPos2);

			void handleTranslate(int x, int y, int state);

			void handleRotation(int x, int y, int state);

			void handleScale(int x, int y, int state);

			void handleSelectPoint(int x, int y, int state);

			float snap(float value, float d);

			void snapVec3(core::vector3df& v);

			core::quaternion getRotation(const core::matrix4& mat);
		};
	}
}
