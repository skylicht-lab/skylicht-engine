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

#include "Entity/IEntityData.h"
#include "Entity/IEntitySystem.h"

#include "CSelectObjectData.h"
#include "Transform/CWorldTransformData.h"
#include "Transform/CWorldInverseTransformData.h"

#include "Camera/CCamera.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSelectObjectSystem : public IEntitySystem
		{
		protected:
			core::array<CSelectObjectData*> m_results;

			core::array<CSelectObjectData*> m_collision;
			core::array<CWorldTransformData*> m_transform;

			CCamera* m_camera;
			core::recti m_viewport;

			bool m_skipUpdate;

		public:
			CSelectObjectSystem();

			virtual ~CSelectObjectSystem();

			virtual void beginQuery(CEntityManager* entityManager);

			virtual void onQuery(CEntityManager* entityManager, CEntity* entity);

			virtual void init(CEntityManager* entityManager);

			virtual void update(CEntityManager* entityManager);

			inline void setCameraAndViewport(CCamera* camera, const core::recti& viewport)
			{
				m_camera = camera;
				m_viewport = viewport;
			}

			inline CCamera* getCamera()
			{
				return m_camera;
			}

			inline const core::recti& getViewport()
			{
				return m_viewport;
			}

			inline int getViewportWidth()
			{
				return m_viewport.getWidth();
			}

			inline int getViewportHeight()
			{
				return m_viewport.getHeight();
			}

			inline core::array<CSelectObjectData*>& getCulledCollision()
			{
				return m_results;
			}
		};
	}
}