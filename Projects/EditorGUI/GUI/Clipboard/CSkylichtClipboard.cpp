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

#include "pch.h"
#include "Utils/CStringImp.h"
#include "CSkylichtClipboard.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtClipboard::CSkylichtClipboard()
			{

			}

			CSkylichtClipboard::~CSkylichtClipboard()
			{

			}

			void CSkylichtClipboard::copyTextToClipboard(const std::wstring& string)
			{
				IOSOperator *os = getIrrlichtDevice()->getOSOperator();
				if (os->isSupportUnicodeClipboard())
				{
					os->copyUnicodeToClipboard(string.c_str());
				}
				else
				{
					int size = CStringImp::getUTF8StringSize(string.c_str());
					if (size > 0)
					{
						char *utf8 = new char[size + 1];
						memset(utf8, 0, size + 1);

						CStringImp::convertUnicodeToUTF8(string.c_str(), utf8);

						os->copyToClipboard(utf8);
						CClipboard::copyTextToClipboard(string);

						delete utf8;
					}
				}
			}

			std::wstring CSkylichtClipboard::getTextFromClipboard()
			{
				IOSOperator *os = getIrrlichtDevice()->getOSOperator();
				if (os->isSupportUnicodeClipboard())
				{
					const wchar_t* utf16 = os->getUnicodeTextFromClipboard();
					if (utf16 == NULL)
					{
						m_text = L"";
						return m_text;
					}
					else
					{
						m_text = utf16;
						return m_text;
					}
				}
				else
				{
					const c8* utf8 = os->getTextFromClipboard();
					if (utf8 == NULL)
					{
						m_text = L"";
						return m_text;
					}

					int size = CStringImp::getUnicodeStringSize(utf8);
					if (size > 0)
					{
						wchar_t *unicodeString = new wchar_t[size + 1];
						memset(unicodeString, 0, size + 1);

						CStringImp::convertUTF8ToUnicode(utf8, unicodeString);
						m_text = unicodeString;

						delete unicodeString;
					}
					else
					{
						m_text = L"";
					}
				}

				return m_text;
			}
		}
	}
}