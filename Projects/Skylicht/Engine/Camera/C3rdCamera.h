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
	/**
	 * @brief This is an object class that provides additional support for the camera, such as looking at a specific target or rotating around that target object.
	 * @ingroup Camera
	 * 
	 * The 3rd person camera support touch screen gestures or mouse dragging to rotate around the target.
	 * You can call the function CCamera::setInputReceiver(false) to disable this feature.
	 * 
	 * Example of setting up a follow camera
	 * 
	 * @code
	 * // create follow 3rd top camera
	 * CGameObject* camera3rdTopObj = zone->createEmptyObject();
	 * CCamera* camera3rdTop = camera3rdTopObj->addComponent<CCamera>();
	 * camera3rdTop->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	 * camera3rdTop->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));
	 * camera3rdTop->setInputReceiver(false);
	 * 
	 * core::vector3df targetOffset(0.0f, 0.5f, 0.0f);
	 * 
	 * C3rdCamera* followTopCam = camera3rdTopObj->addComponent<C3rdCamera>();
	 * followTopCam->setOrientation(45.0f, -45.0f);
	 * followTopCam->setTargetDistance(5.0f);
	 * followTopCam->setTargetOffset(targetOffset);
	 * followTopCam->setFollowTarget(getFollowEntity());
	 * @endcode
	 * 
	 * @see CCamera
	 */
	class SKYLICHT_API C3rdCamera :
		public CComponentSystem,
		public IEventReceiver,
		public ILateUpdate
	{
	public:

	protected:
		//! Pointer to the CCamera component.
		CCamera* m_camera;

		//! The entity being followed.
		CEntity* m_followEntity;

		//! The position being followed (if m_isFollowPosition is true).
		core::vector3df m_followPosition;
		
		//! Offset from the follow target.
		core::vector3df m_targetOffset;

		//! Whether to follow a static position or an entity.
		bool m_isFollowPosition;

		//! Minimum vertical tilt angle.
		float m_minVerticalAngle;
		
		//! Maximum vertical tilt angle.
		float m_maxVerticalAngle;

		//! Horizontal rotation angle (0-360).
		float m_camPan;

		//! Vertical rotation angle (-89 to +89).
		float m_camTilt;

		//! Distance from the target.
		float m_targetDistance;

		//! Tracking touch ID for rotation.
		int m_touchId;

		//! Normalized cursor position at start of drag.
		core::position2df m_centerCursor;
		
		//! Current normalized cursor position.
		core::position2df m_cursorPos;

		//! Whether the primary mouse button is pressed.
		bool m_leftMousePress;

		//! Rotation sensitivity.
		float m_rotateSpeed;

	public:
		C3rdCamera();

		virtual ~C3rdCamera();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void lateUpdate();

		virtual bool OnEvent(const SEvent& event);

		/**
		 * @brief Gets the attached camera component.
		 * @return Pointer to CCamera.
		 */
		inline CCamera* getCamera()
		{
			return m_camera;
		}

		/**
		 * @brief Sets a static world position for the camera to orbit.
		 * @param pos Position vector.
		 */
		inline void setFollowPosition(const core::vector3df& pos)
		{
			m_followPosition = pos;
			m_isFollowPosition = true;
		}

		/**
		 * @brief Gets the static orbit position.
		 * @return Position vector.
		 */
		inline const core::vector3df& getFollowPosition()
		{
			return m_followPosition;
		}

		/**
		 * @brief Sets a GameObject as the orbit target.
		 * @param object Pointer to target object.
		 */
		inline void setFollowTarget(CGameObject* object)
		{
			m_followEntity = object->getEntity();
			m_isFollowPosition = false;
		}

		/**
		 * @brief Sets an entity as the orbit target.
		 * @param entity Pointer to target entity.
		 */
		inline void setFollowTarget(CEntity* entity)
		{
			m_followEntity = entity;
			m_isFollowPosition = false;
		}

		/**
		 * @brief Configures initial orbit orientation and distance.
		 * @param pan Horizontal angle.
		 * @param tilt Vertical angle.
		 * @param distance Distance from target.
		 */
		inline void setOrientation(float pan, float tilt, float distance)
		{
			m_camPan = pan;
			m_camTilt = tilt;
			m_targetDistance = distance;
		}

		/**
		 * @brief Configures initial orbit orientation.
		 * @param pan Horizontal angle.
		 * @param tilt Vertical angle.
		 */
		inline void setOrientation(float pan, float tilt)
		{
			m_camPan = pan;
			m_camTilt = tilt;
			m_camTilt = core::clamp(m_camTilt, m_minVerticalAngle, m_maxVerticalAngle);
		}

		/**
		 * @brief Sets vertical rotation limits.
		 * @param min Min tilt angle.
		 * @param max Max tilt angle.
		 */
		inline void setMinMaxVerticaAngle(float min, float max)
		{
			m_minVerticalAngle = min;
			m_maxVerticalAngle = max;
		}

		/**
		 * @brief Gets the minimum vertical tilt angle.
		 * @return Angle in degrees.
		 */
		inline float getMinVerticalAngle()
		{
			return m_minVerticalAngle;
		}

		/**
		 * @brief Gets the maximum vertical tilt angle.
		 * @return Angle in degrees.
		 */
		inline float getMaxVerticalAngle()
		{
			return m_maxVerticalAngle;
		}

		/**
		 * @brief Sets the distance from the target.
		 * @param d Distance.
		 */
		inline void setTargetDistance(float d)
		{
			m_targetDistance = d;
		}

		/**
		 * @brief Gets the current horizontal rotation.
		 * @return Pan angle.
		 */
		inline float getCameraPan()
		{
			return m_camPan;
		}

		/**
		 * @brief Gets the current vertical rotation.
		 * @return Tilt angle.
		 */
		inline float getCameraTilt()
		{
			return m_camTilt;
		}

		/**
		 * @brief Gets the current distance from the target.
		 * @return Distance.
		 */
		inline float getTargetDistance()
		{
			return m_targetDistance;
		}

		/**
		 * @brief Sets an offset from the follow target (e.g., to frame the character).
		 * @param offset Offset vector.
		 */
		inline void setTargetOffset(const core::vector3df& offset)
		{
			m_targetOffset = offset;
		}

		/**
		 * @brief Gets the target offset.
		 * @return Offset vector.
		 */
		inline const core::vector3df& getTargetOffset()
		{
			return m_targetOffset;
		}

		DECLARE_GETTYPENAME(C3rdCamera)

	protected:

		/**
		 * @brief Internal helper to process rotation input.
		 * @param timeDiff Elapsed time.
		 */
		void updateInputRotate(float timeDiff);

	};
}