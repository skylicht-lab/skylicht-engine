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
			core::vector3df m_position;
			core::quaternion m_rotation;

			bool m_handlePosition;
			bool m_handleRotation;

			core::vector3df m_translateOffset;

			int m_mouseState;

			bool m_endCheck;

		public:
			CHandles();

			virtual ~CHandles();

			bool endCheck();

			void end();

			core::vector3df positionHandle(const core::vector3df& position);

			core::vector3df scaleHandle(const core::vector3df& scale, const core::vector3df& origin);

			core::quaternion rotateHandle(const core::quaternion& rotate, const core::vector3df& origin);

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

			const core::vector3df& getHandlePosition()
			{
				return m_position;
			}

			void setTranslateOffset(const core::vector3df& offset)
			{
				m_translateOffset = offset;
			}

			const core::quaternion& getHandleRotation()
			{
				return m_rotation;
			}

			void setEndCheck(bool b)
			{
				m_endCheck = b;
			}
		};
	}
}