/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "CBase.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CRulerBar : public CBase
			{
			protected:
				bool m_isHorizontal;

				float m_beginOffset;
				float m_position;
				float m_pixelPerUnit;
				float m_unitScale;
				int m_textPerUnit;

				float m_cursorPosition;
				bool m_drawCursorLine;

			public:
				std::function<void(float x, float y, bool down, float value)> OnMouseClickLeftValue;
				std::function<void(float x, float y, bool down, float value)> OnMouseClickRightValue;

			public:
				CRulerBar(CBase* parent, bool horizontal);

				virtual ~CRulerBar();

				virtual void render();

				virtual void onMouseClickLeft(float x, float y, bool down);

				virtual void onMouseClickRight(float x, float y, bool down);

				inline void setBeginOffset(float offset)
				{
					m_beginOffset = offset;
				}

				inline float getBeginOffset()
				{
					return m_beginOffset;
				}

				inline void setPosition(float pos)
				{
					m_position = pos;
				}

				inline float getPosition()
				{
					return m_position;
				}

				inline void setUnitScale(float s)
				{
					m_unitScale = s;
				}

				inline float getUnitScale()
				{
					return m_unitScale;
				}

				inline void setTextPerUnit(int value)
				{
					m_textPerUnit = value;
				}

				inline void setPixelPerUnit(float value)
				{
					m_pixelPerUnit = value;
				}

				inline float getPixelPerUnit()
				{
					return m_pixelPerUnit;
				}

				inline void enableDrawCursorline(bool b)
				{
					m_drawCursorLine = b;
				}

				inline void setCursorPosition(float p)
				{
					m_cursorPosition = p;
				}
			};
		}
	}
}