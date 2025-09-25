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
	/// @brief This is an object class that makes it easier to switch between multiple cameras.
	/// @ingroup Camera
	/// 
	/// You can use CTween and CTweenManager to make the camera transitions smoother.
	/// 
	/// Example of setting up a brain camera
	/// @code
	/// // camera 1
	/// CGameObject* cam1Obj = zone->createEmptyObject();
	/// CCamera* camera1 = cam1Obj->addComponent<CCamera>();
	/// camera1->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	/// camera1->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), Transform::Oy);
	/// 
	/// // camera 2
	/// CGameObject* cam2Obj = zone->createEmptyObject();
	/// CCamera* camera2 = cam2Obj->addComponent<CCamera>();
	/// camera2->setPosition(core::vector3df(3.0f, 1.8f, 3.0f));
	/// camera2->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), Transform::Oy);
	/// 
	/// // create main camera
	/// CGameObject* cameraMainObj = zone->createEmptyObject();
	/// CCamera* cameraMain = cameraMainObj->addComponent<CCamera>();
	/// 
	/// CCameraBrain* g_cameraBrain = cameraMainObj->addComponent<CCameraBrain>();
	/// setTargetCamera(camera1, 0.0f);
	/// setTargetCamera(camera2, 5000.0f);
	/// 
	/// void setTargetCamera(CCamera* cam, float blendDuration)
	/// {
	/// 	// focus new camera
	/// 	g_cameraBrain->setTargetCamera(cam, blendDuration <= 0.0f ? 1.0f: 0.0f);
	/// 
	/// 	if (g_cameraBlendTween)
	/// 	{
	/// 		g_cameraBlendTween->stop();
	/// 		g_cameraBlendTween = NULL;
	/// 	}
	///		if (blendDuration > 0.0f)
	///		{
	///			// create a tween animation from 0.0 to 1.0 over a duration of blendDuration
	/// 		g_cameraBlendTween = new CTweenFloat(0.0f, 1.0f, blendDuration);
	/// 		g_cameraBlendTween->setEase(EaseOutCubic);
	/// 		g_cameraBlendTween->OnUpdate = [&](CTween* tween) {	g_cameraBrain->setBlendValue(g_cameraBlendTween->getValue()); };
	/// 		g_cameraBlendTween->OnFinish = [&](CTween* tween) {	g_cameraBlendTween = NULL; };
	/// 		CTweenManager::getInstance()->addTween(g_cameraBlendTween);
	///		}
	///		// you can call force update a frame
	///		g_cameraBrain->lateUpdate();
	/// }
	/// @endcode
	/// 
	/// @see CCamera
	class SKYLICHT_API CCameraBrain :
		public CComponentSystem,
		public ILateUpdate
	{
	protected:
		CCamera* m_camera;
		CCamera* m_targetCamera;
		float m_blend;

		core::vector3df m_position;
		core::vector3df m_lookAt;
		core::vector3df m_upVector;

		float m_viewFov;
		float m_viewNear;
		float m_viewFar;

		core::vector3df m_lastPosition;
		core::vector3df m_lastLookAt;
		core::vector3df m_lastUpVector;

		float m_lastFov;
		float m_lastNear;
		float m_lastFar;

	public:
		CCameraBrain();

		virtual ~CCameraBrain();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void lateUpdate();

		void setTargetCamera(CCamera* cam, float blendTarget = 1.0f);

		inline CCamera* getTargetCamera()
		{
			return m_targetCamera;
		}

		inline void setBlendValue(float value)
		{
			m_blend = value;
		}

		inline float getBlendValue()
		{
			return m_blend;
		}

		inline const core::vector3df& getPosition()
		{
			return m_position;
		}

		inline const core::vector3df& getLookAt()
		{
			return m_lookAt;
		}

		inline const core::vector3df& getUp()
		{
			return m_upVector;
		}
	};
}