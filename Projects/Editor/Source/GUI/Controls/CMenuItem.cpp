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
#include "CMenuItem.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CMenuItem::CMenuItem(CBase *parent) :
				CButton(parent),
				m_accelerator(NULL),
				m_inMenuBar(false)
			{
				setLabelColor(CThemeConfig::DefaultTextColor);
			}

			CMenuItem::~CMenuItem()
			{

			}

			void CMenuItem::sizeToContents()
			{
				CButton::sizeToContents();

				if (m_accelerator != NULL)
				{
					m_accelerator->sizeToContents();
					setWidth(width() + m_accelerator->width());
				}
			}

			void CMenuItem::renderUnder()
			{

			}

			void CMenuItem::setAccelerator(const std::wstring& accelerator)
			{
				if (m_accelerator != NULL)
				{
					m_accelerator->remove();
					m_accelerator = NULL;
				}

				if (accelerator.empty())
					return;

				m_accelerator = new CTextContainer(this);
				m_accelerator->dock(EPosition::Right);
				m_accelerator->setMargin(SMargin(0.0f, 2.0f, 0.0f, 0.0f));
				m_accelerator->setColor(getLabelColor());
			}
		}
	}
}