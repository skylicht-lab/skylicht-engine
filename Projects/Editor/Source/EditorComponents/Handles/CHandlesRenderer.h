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

#pragma once

#include "Entity/IRenderSystem.h"
#include "CHandlesData.h"

namespace Skylicht
{
	namespace Editor
	{
		class CHandlesRenderer : public IRenderSystem
		{
		protected:
			CHandlesData* m_data;

			bool m_enable;

			float m_screenFactor;

			core::vector3df m_directionUnary[3];
			video::SColor m_directionColor[3];
			float m_axisFactor[3];
			bool m_belowAxisLimit[3];
			bool m_belowPlaneLimit[3];

			bool m_allowAxisFlip;
		public:
			CHandlesRenderer();

			virtual ~CHandlesRenderer();

			virtual void beginQuery();

			virtual void onQuery(CEntityManager* entityManager, CEntity* entity);

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

		protected:

			void drawRotationGizmo(const core::vector3df& pos, const core::vector3df& cameraPos);

			void drawScaleGizmo(const core::vector3df& pos, const core::vector3df& cameraLook, const core::vector3df& cameraUp, CCamera* camera);

			void drawTranslateGizmo(const core::vector3df& pos, const core::vector3df& cameraLook, CCamera* camera);

			float getSegmentLengthClipSpace(const core::vector3df& start, const core::vector3df& end, CCamera* camera);

			float getParallelogram(const core::vector3df& ptO, const core::vector3df& ptA, const core::vector3df& ptB, CCamera* camera);

			void computeTripodAxisAndVisibility(int axisIndex, core::vector3df& dirAxis, core::vector3df& dirPlaneX, core::vector3df& dirPlaneY, bool& belowAxisLimit, bool& belowPlaneLimit, CCamera* camera);
		};
	}
}