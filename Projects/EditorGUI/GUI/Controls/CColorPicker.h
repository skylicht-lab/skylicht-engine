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

#include "CButton.h"
#include "CMenu.h"
#include "CColorHSVRGBPicker.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CColorPicker : public CButton
			{
			public:
				Listener OnChanged;
				Listener OnEndValue;

			protected:

				CMenu* m_menuPicker;

				CColorHSVRGBPicker* m_hsvrgbPicker;

				SGUIColor m_color;

			public:
				CColorPicker(CBase* parent);

				virtual ~CColorPicker();

				virtual void renderUnder();

				void setColor(const SGUIColor& color);

				inline const SGUIColor& getColor()
				{
					return m_color;
				}

			protected:

				void onClickDown(CBase* sender);

				void onSelectColorModify(CBase* sender);

				void onSelectColorOK(CBase* sender);

				void onSelectColorCancel(CBase* sender);
			};
		}
	}
}