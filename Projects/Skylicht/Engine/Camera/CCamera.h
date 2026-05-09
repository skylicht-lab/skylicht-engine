/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#include "Utils/CActivator.h"
#include "Components/CComponentSystem.h"
#include "Transform/CTransform.h"

namespace Skylicht
{
	/**
	 * @brief This is an object class used to set up the camera, including its position, viewing angle, and viewing distance.
	 * @ingroup Camera
	 * 
	 * Example of setting up a 3D camera
	 * 
	 * @code
	 * CGameObject* camObj = zone->createEmptyObject();
	 * CCamera* camera = camObj->addComponent<CCamera>();
	 * 
	 * // setup camera position
	 * camera->setPosition(core::vector3df(10.0f, 5.0f, 10.0f));
	 * // setup camera target
	 * camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));
	 * 
	 * ..
	 * ..
	 * 
	 * // and render the scene with the camera that has been set up
	 * getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti())
	 * @endcode
	 * 
	 * Example of setting up a 2D orthographic camera for drawing a GUI
	 * 
	 * @code
	 * CGameObject* guiCameraObj = zone->createEmptyObject();
	 * guiCameraObj->addComponent<CCamera>();
	 * CCamera* guiCamera = guiCameraObj->getComponent<CCamera>();
	 * guiCamera->setProjectionType(CCamera::OrthoUI);
	 * 
	 * ...
	 * ...
	 * // and render the GUI with the 2D camera.
	 * CGraphics2D::getInstance()->render(guiCamera);
	 * @endcode
	 */
	class SKYLICHT_API CCamera : public CComponentSystem
	{
	public:
		/**
		 * @brief Supported projection modes for the camera.
		 */
		enum ECameraProjection
		{
			Ortho,       //! Orthographic projection (for 2D or stylized 3D).
			Frustum,     //! Custom frustum projection.
			Perspective, //! Standard perspective projection.
			OrthoUI,     //! Screen-space orthographic projection for UI.
			Custom,      //! Manually set projection matrix.
		};

	protected:
		//! Current projection mode.
		ECameraProjection m_projectionType;

		//! Near clipping plane distance.
		float m_nearValue;
		
		//! Far clipping plane distance.
		float m_farValue;
		
		//! Field of View in degrees.
		float m_fov;
		
		//! Aspect ratio (width/height).
		float m_aspect;
		
		//! Actual viewport aspect ratio from render target.
		float m_viewportAspect;

		//! Whether a custom orthographic size is used for OrthoUI.
		bool m_customOrthoSize;
		float m_orthoUIW;
		float m_orthoUIH;

		//! Flag indicating the projection matrix needs recalculation.
		bool m_projectionChanged;

		//! Scale factor for orthographic projection.
		float m_orthoScale;

		//! Current 'up' vector for the view matrix.
		core::vector3df m_up;

		//! Internal view frustum data (planes, matrices).
		SViewFrustum m_viewArea;

		//! Last known screen size.
		core::dimension2du m_screenSize;

		//! Whether this camera receives input events.
		bool m_inputReceiver;

		//! Bitmask for layer-based culling.
		u32 m_cullingMask;

		//! Whether to use scaled or unscaled time for updates.
		bool m_useScaledTime;

	public:
		CCamera();

		virtual ~CCamera();

		virtual void initComponent();

		virtual void updateComponent();

		/**
		 * @brief Finalizes transform and recalculates matrices. Called at the end of the frame update.
		 */
		virtual void endUpdate();

	public:

		/**
		 * @brief Sets the projection mode.
		 * @param projection Mode to set.
		 */
		void setProjectionType(ECameraProjection projection);

		/**
		 * @brief Gets the current projection mode.
		 * @return ECameraProjection.
		 */
		inline ECameraProjection getProjectionType()
		{
			return m_projectionType;
		}

		/**
		 * @brief Returns the calculated projection matrix.
		 * @return 4x4 matrix.
		 */
		const core::matrix4& getProjectionMatrix() const;

		/**
		 * @brief Returns the calculated view matrix.
		 * @return 4x4 matrix.
		 */
		const core::matrix4& getViewMatrix() const;

		/**
		 * @brief Manually sets the view matrix and camera position.
		 * @param view The 4x4 view matrix.
		 * @param position World position of the camera.
		 */
		void setViewMatrix(const core::matrix4& view, const core::vector3df& position);

		/**
		 * @brief Manually sets the projection matrix. Switches mode to 'Custom'.
		 * @param prj The 4x4 projection matrix.
		 */
		void setProjectionMatrix(const core::matrix4& prj);

		/**
		 * @brief Sets the world position of the camera.
		 * @param position World position vector.
		 */
		void setPosition(const core::vector3df& position);

		/**
		 * @brief Gets the current world position of the camera.
		 * @return Position vector.
		 */
		core::vector3df getPosition();

		/**
		 * @brief Copies all projection parameters from another camera.
		 * @param target Source camera.
		 */
		void copyProjection(CCamera* target);

		/**
		 * @brief Configures the camera to look at a target from a specific position.
		 * @param position World position of the camera.
		 * @param target World position of the look-at target.
		 * @param up Up vector (default: Y-up).
		 */
		void lookAt(const core::vector3df& position, const core::vector3df& target, const core::vector3df& up);

		/**
		 * @brief Configures the camera to look at a target from its current position.
		 * @param target World position of the look-at target.
		 * @param up Up vector (default: Y-up).
		 */
		void lookAt(const core::vector3df& target, const core::vector3df& up);

		/**
		 * @brief Sets the 'up' vector for view matrix calculation.
		 * @param up Up vector.
		 */
		void setUpVector(const core::vector3df& up);

		/**
		 * @brief Rotates the camera to point in a specific direction.
		 * @param look Direction vector.
		 */
		void setLookVector(core::vector3df look);

		/**
		 * @brief Gets the current 'up' vector.
		 * @return Up vector.
		 */
		inline const core::vector3df& getUpVector()
		{
			return m_up;
		}

		/**
		 * @brief Gets the current forward viewing direction.
		 * @return Direction vector.
		 */
		core::vector3df getLookVector();

		/**
		 * @brief Sets the near clipping plane distance.
		 * @param f Distance.
		 */
		inline void setNearValue(float f)
		{
			m_nearValue = f;
			m_projectionChanged = true;
		}

		/**
		 * @brief Gets the near clipping plane distance.
		 * @return Distance.
		 */
		inline float getNearValue()
		{
			return m_nearValue;
		}

		/**
		 * @brief Sets the far clipping plane distance.
		 * @param f Distance.
		 */
		inline void setFarValue(float f)
		{
			m_farValue = f;
			m_projectionChanged = true;
		}

		/**
		 * @brief Gets the far clipping plane distance.
		 * @return Distance.
		 */
		inline float getFarValue()
		{
			return m_farValue;
		}

		/**
		 * @brief Sets the vertical Field of View (FOV).
		 * @param fov FOV in degrees.
		 */
		inline void setFOV(float fov)
		{
			m_fov = fov;
			m_projectionChanged = true;
		}

		/**
		 * @brief Gets the vertical FOV.
		 * @return FOV in degrees.
		 */
		inline float getFOV()
		{
			return m_fov;
		}

		/**
		 * @brief Sets the scale for orthographic projection.
		 * @param s Scale factor.
		 */
		inline void setOrthoScale(float s)
		{
			m_orthoScale = s;
		}

		/**
		 * @brief Gets the orthographic scale factor.
		 * @return Scale.
		 */
		inline float getOrthoScale()
		{
			return m_orthoScale;
		}

		/**
		 * @brief Sets a custom viewport size for OrthoUI projection.
		 * @param w Width.
		 * @param h Height.
		 */
		inline void setOrthoUISize(float w, float h)
		{
			m_orthoUIH = h;
			m_orthoUIW = w;
		}

		/**
		 * @brief Enables or disables custom OrthoUI sizing.
		 * @param b True to use custom size, false to use screen size.
		 */
		inline void enableCustomOrthoUISize(bool b)
		{
			m_customOrthoSize = b;
		}

		/**
		 * @brief Sets the aspect ratio. Set to -1 to auto-calculate from viewport.
		 * @param f Aspect ratio (W/H).
		 */
		inline void setAspect(float f)
		{
			m_aspect = f;
			m_projectionChanged = true;
		}

		/**
		 * @brief Gets the effective aspect ratio.
		 * @return Aspect ratio.
		 */
		inline float getAspect()
		{
			if (m_aspect > 0)
				return m_aspect;

			return m_viewportAspect;
		}

		/**
		 * @brief Gets the bitmask used for culling entities.
		 * @return Bitmask.
		 */
		inline u32 getCullingMask()
		{
			return m_cullingMask;
		}

		/**
		 * @brief Sets the bitmask for entity culling.
		 * @param mask Bitmask.
		 */
		inline void setCullingMask(u32 mask)
		{
			m_cullingMask = mask;
		}

		/**
		 * @brief Forces recalculation of the projection matrix.
		 */
		void recalculateProjectionMatrix();

		/**
		 * @brief Forces recalculation of the view matrix.
		 */
		void recalculateViewMatrix();

		/**
		 * @brief Gets the view frustum (planes and matrices).
		 * @return Reference to SViewFrustum.
		 */
		const SViewFrustum& getViewFrustum()
		{
			return m_viewArea;
		}

		/**
		 * @brief Enables or disables input reception for this camera.
		 * @param b Input status.
		 */
		inline void setInputReceiver(bool b)
		{
			m_inputReceiver = b;
		}

		/**
		 * @brief Checks if the camera is receiving input.
		 * @return True if input is enabled.
		 */
		inline bool isInputReceiverEnabled()
		{
			return m_inputReceiver;
		}

		/**
		 * @brief Sets whether camera updates should use the engine's time scale.
		 * @param b True for scaled time, false for raw time.
		 */
		inline void setUseScaledTime(bool b)
		{
			m_useScaledTime = b;
		}

		/**
		 * @brief Checks if the camera uses scaled time.
		 * @return True if using scaled time.
		 */
		inline bool isUseScaledTime()
		{
			return m_useScaledTime;
		}

		DECLARE_GETTYPENAME(CCamera)
	};
}