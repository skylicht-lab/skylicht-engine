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

#include "CCamera.h"
#include "Utils/CActivator.h"
#include "Components/CComponentSystem.h"

namespace Skylicht
{
	/// @brief The object class supports camera movement with arrow keys.
	/// @ingroup Camera
	class SKYLICHT_API CFpsMoveCamera :
		public CComponentSystem,
		public IEventReceiver
	{
	public:
		/**
		 * @brief Directions for FPS-style movement.
		 */
		enum EMoveDirection
		{
			MoveForward = 0,
			MoveBackward,
			StrafeLeft,
			StrafeRight,
			DirectionCount
		};

		/**
		 * @brief Mapping between a direction and a keyboard key.
		 */
		struct SKeyMap
		{
			EMoveDirection Direction;
			EKEY_CODE KeyCode;
		};

	protected:
		//! Pointer to the CCamera component.
		CCamera* m_camera;

		//! List of key mappings for movement.
		std::vector<SKeyMap> m_keyMap;

		//! Array tracking the pressed state of each move direction.
		bool m_input[DirectionCount];

		//! Movement speed multiplier.
		f32 m_moveSpeed;

	public:
		CFpsMoveCamera();

		virtual ~CFpsMoveCamera();

		virtual void initComponent();

		virtual void updateComponent();

		/**
		 * @brief Updates the camera position based on input state. Called at the end of the frame.
		 */
		virtual void endUpdate();

		virtual bool OnEvent(const SEvent& event);

		DECLARE_GETTYPENAME(CFpsMoveCamera)

	public:

		/**
		 * @brief Sets a custom key map for movement.
		 * @param keyMap Vector of SKeyMap structures.
		 */
		inline void setKeyMap(const std::vector<SKeyMap>& keyMap)
		{
			m_keyMap = keyMap;
		}

		/**
		 * @brief Sets the movement speed.
		 * @param s Speed value.
		 */
		inline void setMoveSpeed(float s)
		{
			m_moveSpeed = s;
		}
	};
}