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

#include "CUIBase.h"

namespace Skylicht
{
	namespace UI
	{
		class CUITextBox : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIText* m_text;

			bool m_editable;

		public:
			std::function<void(CUIBase*)> OnTextChanged;

		public:
			CUITextBox(CUIContainer* container, CGUIElement* element);

			virtual ~CUITextBox();

			inline CGUIElement* getBackground()
			{
				return m_background;
			}

			inline CGUIText* getText()
			{
				return m_text;
			}

			inline void setEditable(bool b)
			{
				m_editable = b;
			}

			inline bool isEditable()
			{
				return m_editable;
			}

			virtual void onPointerHover(float pointerX, float pointerY);

			virtual void onPointerOut(float pointerX, float pointerY);

			virtual void onPointerDown(float pointerX, float pointerY);

			virtual void onPointerUp(float pointerX, float pointerY);

			virtual void onPointerMove(float pointerX, float pointerY);

			virtual void onLostFocus();

			virtual void onKeyEvent(const SEvent& event);
		};
	}
}