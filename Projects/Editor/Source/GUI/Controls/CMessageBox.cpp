/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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

#include "pch.h"
#include "GUI/Theme/CThemeConfig.h"
#include "CMessageBox.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CMessageBox::CMessageBox(CBase* base) :
				CDialogWindow(base, 0.0f, 0.0f, 465.0f, 120.0f)
			{
				m_titleBar->remove();
				m_titleBar = NULL;
				m_icon = NULL;
				m_title = NULL;

				m_innerPanel->remove();
				m_innerPanel = NULL;

				CDragger* dragger = new CDragger(this);
				dragger->setClampInsideParent(false);

				m_innerPanel = dragger;
				m_innerPanel->setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));
				m_innerPanel->setMargin(SMargin(0.0f, 0.0f, 0.0f, 0.0f));
				m_innerPanel->dock(EPosition::Fill);

				m_msgIcon = new CIcon(this, ESystemIcon::Info, true);
				m_msgIcon->dock(EPosition::Left);
				m_msgIcon->setMargin(SPadding(15.0f, 15.0f, 0.0f, 0.0f));
				m_msgIcon->setTransparentMouseInput(true);

				CBase* content = new CBase(this);
				content->dock(EPosition::Fill);
				content->setMargin(SPadding(10.0f, 20.0f, 15.0f, 0.0f));
				content->setTransparentMouseInput(true);

				m_info1 = new CLabel(content);
				m_info1->dock(EPosition::Top);
				m_info1->setString("Message Content");
				m_info1->sizeToContents();

				m_info2 = new CLabel(content);
				m_info2->dock(EPosition::Top);
				m_info2->setMargin(SPadding(0.0f, 5.0f, 0.0f, 0.0f));
				m_info2->setString("Target");
				m_info2->setColor(CThemeConfig::ButtonTextDisableColor);
				m_info2->sizeToContents();

				CBase* bottom = new CBase(content);
				bottom->dock(EPosition::Top);
				bottom->setMargin(SPadding(0.0f, 20.0f, 0.0f, 0.0f));
				bottom->setHeight(20.0f);
				bottom->setTransparentMouseInput(true);

				m_cancel = new CButton(bottom);
				m_cancel->setWidth(110.0f);
				m_cancel->setLabel(L"Cancel");
				m_cancel->dock(EPosition::Right);
				m_cancel->setTextAlignment(ETextAlign::TextCenter);
				m_cancel->OnPress = [dialog = this](CBase* control) {
					dialog->onCloseButtonPress(control);
				};

				m_no = new CButton(bottom);
				m_no->setWidth(110.0f);
				m_no->setLabel(L"No");
				m_no->dock(EPosition::Right);
				m_no->setTextAlignment(ETextAlign::TextCenter);
				m_no->setMargin(SPadding(0.0f, 0.0f, 10.0f, 0.0f));

				m_yes = new CButton(bottom);
				m_yes->setWidth(110.0f);
				m_yes->setLabel(L"Yes");
				m_yes->dock(EPosition::Right);
				m_yes->setTextAlignment(ETextAlign::TextCenter);
				m_yes->setMargin(SPadding(0.0f, 0.0f, 10.0f, 0.0f));

				setCenterPosition();
			}

			CMessageBox::~CMessageBox()
			{

			}
		}
	}
}