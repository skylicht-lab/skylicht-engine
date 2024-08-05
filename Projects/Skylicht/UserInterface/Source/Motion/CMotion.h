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

#include "Graphics2D/GUI/CGUIElement.h"
#include "Tween/CTween.h"
#include "Tween/CTweenManager.h"

namespace Skylicht
{
	namespace UI
	{
		enum class EMotionEvent
		{
			In = 0,
			Out,
			PointerHover,
			PointerOut,
			PointerDown,
			PointerUp,
			NumEvent
		};

		class CMotion
		{
		protected:
			CTween* m_tween;
			CGUIElement* m_gui;

			float m_delay;
			float m_duration;

			EMotionEvent m_event;
			EEasingFunctions m_ease;

			bool m_toDefault;
		public:
			CMotion();

			virtual ~CMotion();

			virtual void init(CGUIElement* gui);

			virtual void start() = 0;

			virtual void stop();

			inline void setTime(float delay, float duration)
			{
				m_delay = delay;
				m_duration = duration;
			}

			inline void setEvent(EMotionEvent e)
			{
				m_event = e;
			}

			inline void setEasingFunction(EEasingFunctions f)
			{
				m_ease = f;
			}
		};
	}
}