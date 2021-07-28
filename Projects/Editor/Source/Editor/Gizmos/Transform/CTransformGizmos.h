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

#include "Transform/CTransformEuler.h"
#include "Editor/Gizmos/CGizmos.h"
#include "Reactive/CSubject.h"

namespace Skylicht
{
	namespace Editor
	{
		class CTransformGizmos :
			public CGizmos,
			public IObserver
		{
		protected:
			std::string m_selectID;
			CGameObject* m_selectObject;
			CTransformEuler* m_transform;

			CSubject<core::vector3df> m_position;
			CSubject<core::quaternion> m_rotation;
			CSubject<core::vector3df> m_scale;

		public:
			CTransformGizmos();

			virtual ~CTransformGizmos();

			virtual void onGizmos();

			virtual void onRemove();

			void setPosition(const core::vector3df& pos);

			CSubject<core::vector3df>& getPosition()
			{
				return m_position;
			}

			CSubject<core::quaternion>& getRotation()
			{
				return m_rotation;
			}

			CSubject<core::vector3df>& getScale()
			{
				return m_scale;
			}

			virtual void OnNotify(ISubject* subject, IObserver* from);
		};
	}
}