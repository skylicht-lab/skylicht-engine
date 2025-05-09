/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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

#include "CUIBase.h"

namespace Skylicht
{
	namespace UI
	{
		class CUIButton : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIText* m_text;

			bool m_isToggle;
			bool m_toggleStatus;

		public:
			std::function<void(CUIBase*, bool)> OnToggle;

		public:
			CUIButton(CUIContainer* container, CGUIElement* element);

			virtual ~CUIButton();

			void setLabel(const char* string);

			void setLabel(const wchar_t* string);

			void setLabelAndChangeWidth(const char* string, float addition);

			void setLabelAndChangeWidth(const wchar_t* string, float addition);

			virtual void onPointerHover(float pointerX, float pointerY);

			virtual void onPointerOut(float pointerX, float pointerY);

			virtual void onPressed();

			inline void setToggleButton(bool b)
			{
				m_isToggle = b;
			}

			inline bool isToggleButton(bool b)
			{
				return m_isToggle;
			}

			virtual void setToggle(bool b, bool invokeEvent = true);

			inline bool isToggle()
			{
				return m_toggleStatus;
			}

			inline CGUIElement* getBackground()
			{
				return m_background;
			}

			inline CGUIText* getText()
			{
				return m_text;
			}
		};
	}
}