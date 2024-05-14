/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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

#include "CCamera.h"

#include "Utils/CActivator.h"

#include "Components/CComponentSystem.h"
#include "Components/ILateUpdate.h"

#include "GameObject/CGameObject.h"
#include "Entity/CEntity.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	class SKYLICHT_API C3rdCamera :
		public CComponentSystem,
		public IEventReceiver,
		public ILateUpdate
	{
	public:

	protected:
		CCamera* m_camera;

		CEntity* m_followEntity;

		core::vector3df m_followPosition;
		core::vector3df m_targetOffset;

		bool m_isFollowPosition;

		// camPan = 0 places camera behind Model
		// camPan range 0 - 360
		float m_camPan;

		// camTilt inputs should be between -89 and +89 (top/down)
		float m_camTilt;

		float m_targetDistance;

		// for multitouch on phone device
		int m_touchId;

		core::position2df m_centerCursor;
		core::position2df m_cursorPos;

		bool m_leftMousePress;

		float m_rotateSpeed;

	public:
		C3rdCamera();

		virtual ~C3rdCamera();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void lateUpdate();

		virtual bool OnEvent(const SEvent& event);

		inline CCamera* getCamera()
		{
			return m_camera;
		}

		inline void setFollowPosition(const core::vector3df& pos)
		{
			m_followPosition = pos;
			m_isFollowPosition = true;
		}

		inline void setFollowTarget(CGameObject* object)
		{
			m_followEntity = object->getEntity();
			m_isFollowPosition = false;
		}

		inline void setFollowTarget(CEntity* entity)
		{
			m_followEntity = entity;
			m_isFollowPosition = false;
		}

		inline void setOrientation(float pan, float tilt, float distance)
		{
			m_camPan = pan;
			m_camTilt = tilt;
			m_targetDistance = distance;
		}

		inline void setOrientation(float pan, float tilt)
		{
			m_camPan = pan;
			m_camTilt = tilt;
		}

		inline void setTargetDistance(float d)
		{
			m_targetDistance = d;
		}

		inline float getCameraPan()
		{
			return m_camPan;
		}

		inline float getCameraTilt()
		{
			return m_camTilt;
		}

		inline float getTargetDistance()
		{
			return m_targetDistance;
		}

		inline void setTargetOffset(const core::vector3df& offset)
		{
			m_targetOffset = offset;
		}

		DECLARE_GETTYPENAME(C3rdCamera)

	protected:

		void updateInputRotate(float timeDiff);

	};
}