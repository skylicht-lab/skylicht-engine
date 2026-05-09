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
#include "CCamera.h"
#include "Components/CComponentSystem.h"

namespace Skylicht
{
	/// @brief This is an object class that can perform mouse operations to move the camera. It is used on operating systems with a mouse, such as Windows, Mac, and within the engine's Editor.
	/// @ingroup Camera
	class SKYLICHT_API CEditorCamera :
		public CComponentSystem,
		public IEventReceiver
	{
	public:
		/**
		 * @brief Supported control styles for the editor camera.
		 */
		enum EControlStyle
		{
			Default, //! Standard WASD/Mouse controls.
			Maya,    //! Alt-based navigation (Alt+LMB Rotate, Alt+MMB Pan, Alt+RMB Zoom).
			Blender  //! Blender-style navigation (MMB Rotate, Shift+MMB Pan, Ctrl+MMB Zoom).
		};

	protected:
		//! Pointer to the CCamera component.
		CCamera* m_camera;

		//! Panning/Movement sensitivity.
		f32 m_moveSpeed;
		
		//! Orbit/Rotation sensitivity.
		f32 m_rotateSpeed;
		
		//! Zooming sensitivity.
		f32 m_zoomSpeed;

		//! Cursor control service for retrieving mouse position.
		gui::ICursorControl* m_cursorControl;

		//! Normalized cursor position at start of interaction.
		core::position2df m_centerCursor;
		
		//! Current normalized cursor position.
		core::position2df m_cursorPos;

		//! Modifier key states.
		bool m_altKeyDown;
		bool m_ctrlKeyDown;
		bool m_shiftKeyDown;
		
		//! Basic mouse button states.
		bool m_leftMousePress;
		bool m_rightMousePress;
		bool m_midMousePress;

		//! Specialized Maya mouse button states.
		bool m_mayaLeftMousePress;
		bool m_mayaRightMousePress;

		//! Mouse wheel state and delta.
		bool m_mouseWhell;
		float m_wheel;

		//! Distance from the pivot point for orbit rotation.
		float m_pivotRotateDistance;
		
		//! Active control style.
		EControlStyle m_controlStyle;

	public:
		CEditorCamera();

		virtual ~CEditorCamera();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void endUpdate();

		virtual bool OnEvent(const SEvent& event);

		DECLARE_GETTYPENAME(CEditorCamera)

	public:
		/**
		 * @brief Sets the orbit rotation speed.
		 * @param speed Speed value.
		 */
		inline void setRotateSpeed(float speed)
		{
			m_rotateSpeed = speed;
		}

		/**
		 * @brief Sets the panning speed.
		 * @param speed Speed value.
		 */
		inline void setMoveSpeed(float speed)
		{
			m_moveSpeed = speed;
		}

		/**
		 * @brief Gets the current panning speed.
		 * @return Speed.
		 */
		inline float getMoveSpeed()
		{
			return m_moveSpeed;
		}

		/**
		 * @brief Gets the current orbit rotation speed.
		 * @return Speed.
		 */
		inline float getRotateSpeed()
		{
			return m_rotateSpeed;
		}

		/**
		 * @brief Gets the zooming speed.
		 * @return Speed.
		 */
		inline float getZoomSpeed()
		{
			return m_zoomSpeed;
		}

		/**
		 * @brief Sets the zooming speed.
		 * @param z Speed value.
		 */
		inline void setZoomSpeed(float z)
		{
			m_zoomSpeed = z;
		}

		/**
		 * @brief Gets the attached camera component.
		 * @return Pointer to CCamera.
		 */
		inline CCamera* getCamera()
		{
			return m_camera;
		}

		/**
		 * @brief Sets the active control style (Maya, Blender, etc.).
		 * @param style Control style.
		 */
		inline void setControlStyle(EControlStyle style)
		{
			m_controlStyle = style;
		}

		/**
		 * @brief Gets the current control style.
		 * @return EControlStyle.
		 */
		inline EControlStyle getControlStyle()
		{
			return m_controlStyle;
		}

		/**
		 * @brief Checks if the right mouse button is currently held.
		 * @return True if pressed.
		 */
		inline bool isRightMousePressed()
		{
			return m_rightMousePress;
		}

		/**
		 * @brief Checks if the left mouse button is currently held.
		 * @return True if pressed.
		 */
		inline bool isLeftMousePressed()
		{
			return m_leftMousePress;
		}

		/**
		 * @brief Sets the orbit pivot point and calculates distance.
		 * @param pos World position of the pivot.
		 */
		void setPivotRotate(const core::vector3df& pos);
		
		/**
		 * @brief Directly sets the distance to the orbit pivot.
		 * @param d Distance.
		 */
		inline void setPivotRotateDistance(float d)
		{
			m_pivotRotateDistance = d;
		}
		
	protected:

		/**
		 * @brief Cleans up tiny vector components to prevent floating point drift.
		 * @param v Vector to fix.
		 */
		void fixVector(core::vector3df& v);
		
		/**
		 * @brief Updates horizontal and vertical orbit rotation.
		 * @param relativeRotation [in/out] Current rotation angles.
		 * @param timeDiff Elapsed time.
		 * @param useCenterPivot Whether to rotate around the pivot point.
		 */
		void updateInputRotate(core::vector3df& relativeRotation, f32 timeDiff, bool useCenterPivot = false);
		
		/**
		 * @brief Updates position offset (panning).
		 * @param offsetPosition [out] Calculated world offset.
		 * @param timeDiff Elapsed time.
		 */
		void updateInputOffset(core::vector3df& offsetPosition, f32 timeDiff);

		/**
		 * @brief Updates zoom by moving the camera along its forward vector.
		 * @param timeDiff Elapsed time.
		 * @param pos [in/out] Camera position.
		 * @param moveDir Forward viewing direction.
		 */
		void updateInputZoom(f32 timeDiff, core::vector3df& pos, const core::vector3df& moveDir);
	};
}
