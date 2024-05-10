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

namespace Skylicht
{
	class SKYLICHT_API CEditorCamera :
		public CComponentSystem,
		public IEventReceiver
	{
	protected:
		f32 m_moveSpeed;
		f32 m_rotateSpeed;
		gui::ICursorControl* m_cursorControl;

		core::position2df m_centerCursor;
		core::position2df m_cursorPos;

		bool m_leftMousePress;
		bool m_rightMousePress;
		bool m_midMousePress;

		bool m_mouseWhell;
		float m_wheel;

	public:
		CEditorCamera();

		virtual ~CEditorCamera();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void endUpdate();

		virtual bool OnEvent(const SEvent& event);

		DECLARE_GETTYPENAME(CEditorCamera)

	public:
		inline void setRotateSpeed(float speed)
		{
			m_rotateSpeed = speed;
		}

		inline void setMoveSpeed(float speed)
		{
			m_moveSpeed = speed;
		}

		inline float getMoveSpeed()
		{
			return m_moveSpeed;
		}

		inline float getRotateSpeed()
		{
			return m_rotateSpeed;
		}

		void updateInputRotate(core::vector3df& relativeRotation, f32 timeDiff);

		void updateInputOffset(core::vector3df& offsetPosition, f32 timeDiff);


	};
}