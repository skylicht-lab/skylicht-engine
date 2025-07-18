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
#include "CText.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CIcon : public CBase
			{
			protected:
				ESystemIcon m_icon;
				SGUIColor m_color;

				bool m_use32px;

			public:
				CIcon(CBase* parent, ESystemIcon icon, bool use32px = false);

				virtual ~CIcon();

				virtual void render();

				inline void setIcon(ESystemIcon icon)
				{
					m_icon = icon;
				}

				inline ESystemIcon getIcon()
				{
					return m_icon;
				}

				inline void setColor(const SGUIColor& c)
				{
					m_color = c;
				}

				inline const SGUIColor& getColor()
				{
					return m_color;
				}

				inline void setLargeIcon(bool b)
				{
					m_use32px = b;
				}
			};
		}
	}
}