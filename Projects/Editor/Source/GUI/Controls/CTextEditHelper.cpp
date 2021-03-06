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
#include "CTextEditHelper.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTextEditHelper::CTextEditHelper(CTextBox* textBox, CTextContainer* textContainer) :
				m_textBox(textBox),
				m_textContainer(textContainer)
			{
				m_textBox->setHidden(true);
				m_textBox->OnChar = BIND_LISTENER(&CTextEditHelper::onChar, this);
			}

			CTextEditHelper::~CTextEditHelper()
			{

			}

			void CTextEditHelper::beginEdit(CBase::Listener onCancel, CBase::Listener onEndEdit)
			{
				updateTextBoxSize();

				m_textBox->setString(m_textContainer->getString());
				m_textBox->setWrapMultiline(m_textContainer->isWrapMultiline());

				m_textBox->setHidden(false);
				m_textContainer->setHidden(true);

				m_onCancel = onCancel;
				m_onEndEdit = onEndEdit;
			}

			void CTextEditHelper::cancelEdit()
			{
				m_textBox->setHidden(true);
				m_textContainer->setHidden(false);

				if (m_onCancel != nullptr)
					m_onCancel(m_textBox);
			}

			void CTextEditHelper::endEnd()
			{
				m_textContainer->setString(m_textBox->getString());

				m_textBox->setHidden(true);
				m_textContainer->setHidden(false);

				if (m_onEndEdit != nullptr)
					m_onEndEdit(m_textBox);
			}

			void CTextEditHelper::onChar(CBase* textBox)
			{
				m_textContainer->setString(m_textBox->getString());
				m_textContainer->layout();
				updateTextBoxSize();
			}

			void CTextEditHelper::updateTextBoxSize()
			{
				SRect bound = m_textContainer->getBounds();
				bound.Y -= 3.0f;
				bound.Height += 5.0f;
				bound.Width += 8.0f;

				m_textBox->setBounds(bound);
			}
		}
	}
}