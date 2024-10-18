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
#include "CButton.h"
#include "CTextBox.h"
#include "CRawImage.h"
#include "CLabel.h"
#include "CSlider.h"

#include "CColorHuePicker.h"
#include "CColorSVPicker.h"
#include "CColorChannelPicker.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CColorHSVRGBPicker : public CBase
			{
			protected:
				CGUIImage* m_hsvImage;
				CGUIImage* m_hueImage;
				CGUIImage* m_colorBGImage;

				CGUIImage* m_redImage;
				CGUIImage* m_greenImage;
				CGUIImage* m_blueImage;
				CGUIImage* m_alphaImage;
				CGUIImage* m_sImage;
				CGUIImage* m_vImage;


				CColorSVPicker* m_sv;
				CColorHuePicker* m_hue;

				CButton* m_buttonOK;
				CButton* m_buttonCancel;

				CTextBox* m_textboxHex;
				CTextBox* m_textboxColor;

				CColorChannelPicker* m_red;
				CColorChannelPicker* m_green;
				CColorChannelPicker* m_blue;
				CColorChannelPicker* m_alpha;

				CColorChannelPicker* m_s;
				CColorChannelPicker* m_v;

				SGUIColor m_color;
				SGUIColor m_oldColor;

				SRect m_previewBounds;

			public:

				Listener OnCancel;
				Listener OnOK;
				Listener OnModify;

			public:
				CColorHSVRGBPicker(CBase* parent);

				virtual ~CColorHSVRGBPicker();

				virtual void renderUnder();

				const SGUIColor& getColor()
				{
					return m_color;
				}

				const SGUIColor& getOldColor()
				{
					return m_oldColor;
				}

				inline void setOldColor(const SGUIColor& c)
				{
					m_oldColor = c;
				}

				void setColor(const SGUIColor& c);

				void changeHue(CBase* base);

				void changeSV(CBase* base);

				void refreshColor();

				void refreshColorUI();

			protected:

				void onRGBAChange(CBase* base);

				void onSVChange(CBase* base);

				void onColorHexChanged(CBase* base);

				void onColorTextChanged(CBase* base);

				void onCancel(CBase* base);

				void onOK(CBase* base);

				void updateColorText();

				void setupHSVBitmap(unsigned char h, unsigned char s, unsigned char v);

				void setupHUEBitmap();

				void setupColorBGBitmap();

				void setupColorChannelBitmap();
			};
		}
	}
}