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
#include "Components/CComponentSystem.h"
#include "Components/ILateUpdate.h"

namespace Skylicht
{
	/**
	 * @brief This is an object class that makes it easier to switch between multiple cameras.
	 * @ingroup Camera
	 * 
	 * You can use CTween and CTweenManager to make the camera transitions smoother.
	 * 
	 * Example of setting up a brain camera
	 * 
	 * @code
	 * // camera 1
	 * CGameObject* cam1Obj = zone->createEmptyObject();
	 * CCamera* camera1 = cam1Obj->addComponent<CCamera>();
	 * camera1->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	 * camera1->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), Transform::Oy);
	 * 
	 * // camera 2
	 * CGameObject* cam2Obj = zone->createEmptyObject();
	 * CCamera* camera2 = cam2Obj->addComponent<CCamera>();
	 * camera2->setPosition(core::vector3df(3.0f, 1.8f, 3.0f));
	 * camera2->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), Transform::Oy);
	 * 
	 * // create main camera
	 * CGameObject* cameraMainObj = zone->createEmptyObject();
	 * CCamera* cameraMain = cameraMainObj->addComponent<CCamera>();
	 * 
	 * CCameraBrain* g_cameraBrain = cameraMainObj->addComponent<CCameraBrain>();
	 * setTargetCamera(camera1, 0.0f);
	 * setTargetCamera(camera2, 5000.0f);
	 * 
	 * void setTargetCamera(CCamera* cam, float blendDuration)
	 * {
	 * 	// focus new camera
	 * 	g_cameraBrain->setTargetCamera(cam, blendDuration <= 0.0f ? 1.0f: 0.0f);
	 * 
	 * 	if (g_cameraBlendTween)
	 * 	{
	 * 		g_cameraBlendTween->stop();
	 * 		g_cameraBlendTween = NULL;
	 * 	}
	 *		if (blendDuration > 0.0f)
	 *		{
	 *			// create a tween animation from 0.0 to 1.0 over a duration of blendDuration
	 * 		g_cameraBlendTween = new CTweenFloat(0.0f, 1.0f, blendDuration);
	 * 		g_cameraBlendTween->setEase(EaseOutCubic);
	 * 		g_cameraBlendTween->OnUpdate = [&](CTween* tween) {	g_cameraBrain->setBlendValue(g_cameraBlendTween->getValue()); };
	 * 		g_cameraBlendTween->OnFinish = [&](CTween* tween) {	g_cameraBlendTween = NULL; };
	 * 		CTweenManager::getInstance()->addTween(g_cameraBlendTween);
	 *		}
	 *		// you can call force update a frame
	 *		g_cameraBrain->lateUpdate();
	 * }
	 * @endcode
	 * 
	 * @see CCamera
	 */
	class SKYLICHT_API CCameraBrain :
		public CComponentSystem,
		public ILateUpdate
	{
	protected:
		//! Pointer to the main camera component being controlled.
		CCamera* m_camera;
		
		//! The target camera to blend towards.
		CCamera* m_targetCamera;
		
		//! Current blending factor (0 to 1).
		float m_blend;

		//! Current blended world position.
		core::vector3df m_position;
		
		//! Current blended forward direction.
		core::vector3df m_lookAt;
		
		//! Current blended up direction.
		core::vector3df m_upVector;

		//! Current blended FOV.
		float m_viewFov;
		
		//! Current blended near plane.
		float m_viewNear;
		
		//! Current blended far plane.
		float m_viewFar;

		//! Last position before starting a new blend.
		core::vector3df m_lastPosition;
		
		//! Last forward direction before starting a new blend.
		core::vector3df m_lastLookAt;
		
		//! Last up direction before starting a new blend.
		core::vector3df m_lastUpVector;

		//! Last FOV before starting a new blend.
		float m_lastFov;
		
		//! Last near plane before starting a new blend.
		float m_lastNear;
		
		//! Last far plane before starting a new blend.
		float m_lastFar;

	public:
		CCameraBrain();

		virtual ~CCameraBrain();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void lateUpdate();

		/**
		 * @brief Sets a new target camera and initiates blending.
		 * @param cam Pointer to the target camera.
		 * @param blendTarget Initial blend value (default: 1.0, immediate switch).
		 */
		void setTargetCamera(CCamera* cam, float blendTarget = 1.0f);

		/**
		 * @brief Gets the main camera component controlled by this brain.
		 * @return Pointer to CCamera.
		 */
		inline CCamera* getCamera()
		{
			return m_camera;
		}

		/**
		 * @brief Gets the current target camera.
		 * @return Pointer to CCamera.
		 */
		inline CCamera* getTargetCamera()
		{
			return m_targetCamera;
		}

		/**
		 * @brief Manually sets the blending factor.
		 * @param value Blending factor (0.0 = old camera, 1.0 = target camera).
		 */
		inline void setBlendValue(float value)
		{
			m_blend = value;
		}

		/**
		 * @brief Gets the current blending factor.
		 * @return Factor (0 to 1).
		 */
		inline float getBlendValue()
		{
			return m_blend;
		}

		/**
		 * @brief Gets the current blended world position.
		 * @return Position vector.
		 */
		inline const core::vector3df& getPosition()
		{
			return m_position;
		}

		/**
		 * @brief Gets the current blended forward look direction.
		 * @return Direction vector.
		 */
		inline const core::vector3df& getLookAt()
		{
			return m_lookAt;
		}

		/**
		 * @brief Gets the current blended 'up' vector.
		 * @return Up vector.
		 */
		inline const core::vector3df& getUp()
		{
			return m_upVector;
		}
	};
}