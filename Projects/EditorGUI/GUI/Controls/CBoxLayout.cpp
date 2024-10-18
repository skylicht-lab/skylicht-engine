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
#include "CBoxLayout.h"

#include <assert.h>

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CBoxLayout::CBoxLayout(CBase* parent) :
				CBase(parent)
			{
				CLayoutHorizontal* layout = new CLayoutHorizontal(this);
				layout->dock(EPosition::Top);
				layout->setPos(0.0f, 0.0f);
				layout->sendToBack();

				dock(EPosition::Top);

				m_layoutStack.push(layout);
			}

			CBoxLayout::~CBoxLayout()
			{

			}

			void CBoxLayout::postLayout()
			{
				CBase::postLayout();

				sizeToChildren(false, true);
			}

			CLayoutHorizontal* CBoxLayout::beginHorizontal()
			{
				CBase* parent = m_layoutStack.top();
				CLayoutHorizontal* h = new CLayoutHorizontal(parent);
				h->dock(EPosition::Top);
				m_layoutStack.push(h);
				return h;
			}

			void CBoxLayout::endHorizontal()
			{
				if (dynamic_cast<CLayoutHorizontal*>(m_layoutStack.top()) != NULL && m_layoutStack.size() > 1)
					m_layoutStack.pop();
#if _DEBUG
				else
					assert(false);
#endif
			}

			void CBoxLayout::addSpace(float height)
			{
				CBase* space = new CBase(m_layoutStack.top());
				space->dock(EPosition::Top);
				space->setHeight(height);
			}

			CLayoutVertical* CBoxLayout::beginVertical()
			{
				CBase* parent = m_layoutStack.top();
				CLayoutVertical* v = new CLayoutVertical(parent);
				v->dock(EPosition::Top);
				m_layoutStack.push(v);
				return v;
			}

			void CBoxLayout::endVertical()
			{
				if (dynamic_cast<CLayoutVertical*>(m_layoutStack.top()) != NULL && m_layoutStack.size() > 1)
					m_layoutStack.pop();
#if _DEBUG
				else
					assert(false);
#endif
			}

			CLayoutColumn* CBoxLayout::beginColumn()
			{
				CBase* parent = m_layoutStack.top();
				CLayoutColumn* v = new CLayoutColumn(parent);
				v->dock(EPosition::Top);
				m_layoutStack.push(v);
				return v;
			}

			void CBoxLayout::endColumn()
			{
				if (dynamic_cast<CLayoutColumn*>(m_layoutStack.top()) != NULL && m_layoutStack.size() > 1)
					m_layoutStack.pop();
#if _DEBUG
				else
					assert(false);
#endif
			}
		}
	}
}