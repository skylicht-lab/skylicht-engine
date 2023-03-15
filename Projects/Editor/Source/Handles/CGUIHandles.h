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

namespace Skylicht
{
	namespace Editor
	{
		class CGUIHandles :
			public CGameSingleton<CGUIHandles>,
			public IEventReceiver
		{
		protected:
			core::matrix4 m_world;
			core::matrix4 m_worldInv;

			core::vector3df m_targetPosition;

		public:
			CGUIHandles();

			virtual ~CGUIHandles();

			core::vector3df positionHandle(const core::vector3df& position, const core::vector3df& localRotation);

			void end();

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

			virtual bool OnEvent(const SEvent& event);
		};
	}
}