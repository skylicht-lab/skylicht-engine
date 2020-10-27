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
#include "CLabel.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CLabel::CLabel(CBase *parent) :
				CBase(parent)
			{
				setMouseInputEnabled(false);
				m_text = new CTextContainer(this);
				m_text->dock(EPosition::Fill);
			}

			CLabel::~CLabel()
			{

			}

			void CLabel::onBoundsChanged(const SRect& oldBounds)
			{
				CBase::onBoundsChanged(oldBounds);

				m_text->refreshSize();
				invalidate();
			}

			void CLabel::setString(const std::wstring& text)
			{
				m_text->setString(text);

				if (OnTextChanged != nullptr)
					OnTextChanged(this);
			}

			void CLabel::setColor(const SGUIColor& color)
			{
				m_text->setColor(color);
			}

			void CLabel::setFontSize(EFontSize fontsize)
			{
				m_text->setFontSize(fontsize);
			}
		}
	}
}