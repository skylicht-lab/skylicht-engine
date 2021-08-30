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

#include "Utils/CGameSingleton.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	namespace Editor
	{
		class CHandles :
			public CGameSingleton<CHandles>,
			public IEventReceiver
		{
		protected:
			core::matrix4 m_world;
			core::matrix4 m_worldInv;

			core::vector3df m_position;
			core::quaternion m_rotation;
			core::vector3df m_scale;

			bool m_handlePosition;
			bool m_handleRotation;
			bool m_handleScale;

			core::vector3df m_targetPosition;
			core::quaternion m_targetRotation;
			core::vector3df m_targetScale;

			int m_mouseState;

			bool m_endCheck;
			bool m_useLocalSpace;

		public:
			CHandles();

			virtual ~CHandles();

			bool endCheck();

			void end();

			core::vector3df positionHandle(const core::vector3df& position, const core::quaternion& localRotation);

			core::vector3df scaleHandle(const core::vector3df& scale, const core::vector3df& origin, const core::quaternion& localRotation);

			core::quaternion rotateHandle(const core::quaternion& rotate, const core::vector3df& origin);

			inline void setUseLocalSpace(bool b)
			{
				m_useLocalSpace = b;
			}

			inline bool useLocalSpace()
			{
				return m_useLocalSpace;
			}

			virtual bool OnEvent(const SEvent& event);

		public:

			inline bool isHandlePosition()
			{
				return m_handlePosition;
			}

			inline bool isHandleRotation()
			{
				return m_handleRotation;
			}

			inline bool isHandleScale()
			{
				return m_handleScale;
			}

			inline const core::vector3df& getHandlePosition()
			{
				return m_position;
			}

			inline void setTargetPosition(const core::vector3df& target)
			{
				m_targetPosition = target;
			}

			inline const core::quaternion& getHandleRotation()
			{
				return m_rotation;
			}

			inline void setTargetRotation(const core::quaternion& target)
			{
				m_targetRotation = target;
			}

			inline const core::vector3df& getHandleScale()
			{
				return m_scale;
			}

			inline void setTargetScale(const core::vector3df& scale)
			{
				m_targetScale = scale;
			}

			inline void setEndCheck(bool b)
			{
				m_endCheck = b;
			}

			inline void setWorld(const core::matrix4& m)
			{
				m_world = m;
				m.getInverse(m_worldInv);
			}

			inline core::matrix4& getWorld()
			{
				return m_world;
			}

			inline core::matrix4& getWorldInv()
			{
				return m_worldInv;
			}
		};
	}
}