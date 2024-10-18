/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CToggleGroup.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CToggleGroup::CToggleGroup() :
				selected(NULL)
			{

			}

			CToggleGroup::~CToggleGroup()
			{

			}

			void CToggleGroup::addButton(CButton* button)
			{
				if (std::find(m_buttons.begin(), m_buttons.end(), button) != m_buttons.end())
					return;

				button->setToggle(true);
				button->setIsToggle(false);

				m_buttons.push_back(button);
			}

			void CToggleGroup::removeButton(CButton* button)
			{
				std::vector<CButton*>::iterator i = std::find(m_buttons.begin(), m_buttons.end(), button);
				if (i != m_buttons.end())
					m_buttons.erase(i);
			}

			int CToggleGroup::selectButton(CButton* button)
			{
				int result = -1;
				int i = 0;

				for (CButton* b : m_buttons)
				{
					if (b == button)
					{
						b->setIsToggle(true);
						result = i;
					}
					else
						b->setIsToggle(false);

					i++;
				}

				return result;
			}

			CButton* CToggleGroup::getSelectButton()
			{
				for (CButton* b : m_buttons)
				{
					if (b->isToggle())
						return b;
				}

				return NULL;
			}

			void CToggleGroup::enable(bool b)
			{
				for (CButton* button : m_buttons)
				{
					button->setDisabled(!b);
				}
			}
		}
	}
}