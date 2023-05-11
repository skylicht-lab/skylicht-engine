/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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

#include "Reactive/CSubject.h"
#include "Editor/Gizmos/CGizmos.h"
#include "Graphics2D/GUI/CGUIElement.h"

namespace Skylicht
{
	namespace Editor
	{
		class CGUITransformGizmos :
			public CGizmos,
			public IObserver
		{
		protected:
			std::string m_selectID;

			CGUIElement* m_gui;
			core::matrix4 m_parentWorld;

			CSubject<core::vector3df> m_position;
			CSubject<core::quaternion> m_rotation;
			CSubject<core::vector3df> m_scale;
			CSubject<core::rectf> m_rect;

			bool m_changed;

		public:
			CGUITransformGizmos();

			virtual ~CGUITransformGizmos();

			virtual void onGizmos();

			virtual void onEnable();

			virtual void onRemove();

			virtual void refresh();

			virtual void onNotify(ISubject* subject, IObserver* from);

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

			void setTransform(const core::vector3df& pos, const core::vector3df& rot, const core::vector3df& scale, const core::rectf& rect);

		protected:

			void updateProperty();
		};
	}
}