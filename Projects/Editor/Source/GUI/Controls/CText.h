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
			class CText : public CBase
			{
			protected:
				std::wstring m_string;
				EFontSize m_fontSize;
				SGUIColor m_color;

			public:
				CText(CBase *parent);

				virtual ~CText();

				virtual void render();

				void setString(const std::wstring& string);

				SDimension getCharacterPosition(u32 c);

				u32 getClosestCharacter(const SPoint& point);

				inline u32 getLength()
				{
					return m_string.size();
				}

				u32 getLengthNoNewLine()
				{
					u32 length = m_string.size();
					if (length > 0)
					{
						if (m_string[length - 1] == '\n')
							length--;
					}
					return length;
				}

				inline const std::wstring& getString()
				{
					return m_string;
				}

				void setFontSize(EFontSize size);

				inline EFontSize getFontSize()
				{
					return m_fontSize;
				}

				inline void setColor(const SGUIColor& color)
				{
					m_color = color;
				}

				inline const SGUIColor& getColor()
				{
					return m_color;
				}
			};
		}
	}
}