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
#include "CLayoutColumn.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CLayoutColumn::CLayoutColumn(CBase* parent) :
				CLayout(parent)
			{
				m_childPadding = 5.0f;
				setHeight(20.0f);
			}

			CLayoutColumn::~CLayoutColumn()
			{

			}

			void CLayoutColumn::layout()
			{
				int numChild = (int)Children.size();
				if (numChild > 0)
				{
					float x = 0.0f;
					float y = 0.0f;

					int id = 0;

					for (CBase* c : Children)
					{
						float w = c->width();

						x = x + c->getPadding().Left;
						y = c->getPadding().Top;

						if (id == numChild - 1)
							w = width() - x;

						float h = c->height();
						if (h == 0.0f)
							h = height();

						c->setBounds(floorf(x), floorf(y), w, h);
						x = x + w + m_childPadding;
						id++;
					}
				}

				CLayout::layout();
			}

			void CLayoutColumn::postLayout()
			{
				sizeToChildren(false, true);
			}
		}
	}
}