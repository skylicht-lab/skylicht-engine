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

#pragma once

#include "CDialogWindow.h"
#include "CButton.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CMessageBox : public CDialogWindow
			{
			public:
				enum EMessageType
				{
					OK = 0,
					OKCancel,
					YesNo,
					YesNoCancel,
				};

			protected:
				CIcon* m_msgIcon;

				CLabel* m_message1;
				CLabel* m_message2;

				CButton* m_button3;
				CButton* m_button2;
				CButton* m_button1;

				EMessageType m_type;

			public:
				Listener OnCancel;
				Listener OnOK;
				Listener OnYes;
				Listener OnNo;

			public:
				CMessageBox(CBase* base, EMessageType type);

				virtual ~CMessageBox();

				void setMessage(const std::wstring& msg, const std::wstring& target);

				void setMessage(const std::string& msg, const std::string& target);

				inline CButton* getButton1()
				{
					return m_button1;
				}

				inline CButton* getButton2()
				{
					return m_button2;
				}

				inline CButton* getButton3()
				{
					return m_button3;
				}

				inline CIcon* getMessageIcon()
				{
					return m_msgIcon;
				}

				inline EMessageType getType()
				{
					return m_type;
				}
			};
		}
	}
}