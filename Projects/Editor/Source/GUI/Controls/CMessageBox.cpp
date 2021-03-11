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
			CMessageBox::CMessageBox(CBase* base, EMessageType type) :
				CDialogWindow(base, 0.0f, 0.0f, 465.0f, 120.0f),
				m_type(type)
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

				m_message1 = new CLabel(content);
				m_message1->dock(EPosition::Top);
				m_message1->setWrapMultiline(true);
				m_message1->setString(" ");
				m_message1->setHeight(20.0f);

				m_message2 = new CLabel(content);
				m_message2->dock(EPosition::Top);
				m_message2->setMargin(SPadding(0.0f, 5.0f, 0.0f, 0.0f));
				m_message2->setColor(CThemeConfig::ButtonTextDisableColor);
				m_message2->setString(" ");
				m_message2->setHeight(20.0f);

				CBase* bottom = new CBase(content);
				bottom->dock(EPosition::Top);
				bottom->setMargin(SPadding(0.0f, 20.0f, 0.0f, 0.0f));
				bottom->setHeight(20.0f);
				bottom->setTransparentMouseInput(true);

				m_button1 = new CButton(bottom);
				m_button1->setWidth(110.0f);
				m_button1->dock(EPosition::Right);
				m_button1->setTextAlignment(ETextAlign::TextCenter);
				m_button1->OnPress = [dialog = this](CBase* control) {
					CMessageBox::EMessageType type = dialog->getType();
					switch (type)
					{
					case CMessageBox::OK:
						if (dialog->OnOK != nullptr)
							dialog->OnOK(dialog);
						break;
					case CMessageBox::OKCancel:
						if (dialog->OnCancel != nullptr)
							dialog->OnCancel(dialog);
						break;
					case CMessageBox::YesNo:
						if (dialog->OnNo != nullptr)
							dialog->OnNo(dialog);
						break;
					case CMessageBox::YesNoCancel:
						if (dialog->OnCancel != nullptr)
							dialog->OnCancel(dialog);
						break;
					default:
						break;
					}
					dialog->onCloseButtonPress(control);
				};

				m_button2 = new CButton(bottom);
				m_button2->setWidth(110.0f);
				m_button2->dock(EPosition::Right);
				m_button2->setTextAlignment(ETextAlign::TextCenter);
				m_button2->setMargin(SPadding(0.0f, 0.0f, 10.0f, 0.0f));
				m_button2->OnPress = [dialog = this](CBase* control) {
					CMessageBox::EMessageType type = dialog->getType();
					switch (type)
					{
					case CMessageBox::OK:
						break;
					case CMessageBox::OKCancel:
						if (dialog->OnOK != nullptr)
							dialog->OnOK(dialog);
						break;
					case CMessageBox::YesNo:
						if (dialog->OnYes != nullptr)
							dialog->OnYes(dialog);
						break;
					case CMessageBox::YesNoCancel:
						if (dialog->OnNo != nullptr)
							dialog->OnNo(dialog);
						break;
					default:
						break;
					}
					dialog->onCloseButtonPress(control);
				};

				m_button3 = new CButton(bottom);
				m_button3->setWidth(110.0f);
				m_button3->dock(EPosition::Right);
				m_button3->setTextAlignment(ETextAlign::TextCenter);
				m_button3->setMargin(SPadding(0.0f, 0.0f, 10.0f, 0.0f));
				m_button3->OnPress = [dialog = this](CBase* control) {
					CMessageBox::EMessageType type = dialog->getType();
					switch (type)
					{
					case CMessageBox::OK:
						break;
					case CMessageBox::OKCancel:
						break;
					case CMessageBox::YesNo:
						break;
					case CMessageBox::YesNoCancel:
						if (dialog->OnYes != nullptr)
							dialog->OnYes(dialog);
						break;
					default:
						break;
					}
					dialog->onCloseButtonPress(control);
				};

				switch (type)
				{
				case CMessageBox::OK:
					m_button1->setLabel(L"OK");
					m_button2->setHidden(true);
					m_button3->setHidden(true);
					break;
				case CMessageBox::OKCancel:
					m_button1->setLabel(L"Cancel");
					m_button2->setLabel(L"OK");
					m_button3->setHidden(true);
					break;
				case CMessageBox::YesNo:
					m_button1->setLabel(L"No");
					m_button2->setLabel(L"Yes");
					m_button3->setHidden(true);
					break;
				case CMessageBox::YesNoCancel:
					m_button1->setLabel(L"Cancel");
					m_button2->setLabel(L"No");
					m_button3->setLabel(L"Yes");
					break;
				default:
					break;
				}
				setCenterPosition();
			}

			CMessageBox::~CMessageBox()
			{

			}

			void CMessageBox::setMessage(const std::wstring& msg, const std::wstring& target)
			{
				m_message1->setString(msg);
				m_message2->setString(target);

				m_message1->sizeToContents();
				m_message2->sizeToContents();
			}

			void CMessageBox::setMessage(const std::string& msg, const std::string& target)
			{
				m_message1->setString(msg);
				m_message2->setString(target);

				m_message1->sizeToContents();
				m_message2->sizeToContents();
			}
		}
	}
}