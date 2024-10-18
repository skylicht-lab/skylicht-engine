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
#include "CContentSizeControl.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CContentSizeControl::CContentSizeControl(CBase* parent) :
				CBase(parent),
				m_targetParent(parent),
				m_horizontalFit(CContentSizeControl::WrapChildren),
				m_verticalFit(CContentSizeControl::WrapChildren),
				m_parentWidth(0.0f),
				m_parentHeight(0.0f)
			{

			}

			CContentSizeControl::~CContentSizeControl()
			{

			}

			void CContentSizeControl::think()
			{
				if (m_horizontalFit == SizeToParent)
				{
					if (m_parentWidth != m_targetParent->width())
					{
						m_parentWidth = m_targetParent->width();
						invalidate();
					}
				}

				if (m_verticalFit == SizeToParent)
				{
					if (m_parentHeight != m_targetParent->height())
					{
						m_parentHeight = m_targetParent->height();
						invalidate();
					}
				}
			}

			void CContentSizeControl::layout()
			{
				if (m_horizontalFit == SizeToParent)
				{
					setWidth(m_targetParent->width());
				}

				if (m_verticalFit == SizeToParent)
				{
					setHeight(m_targetParent->height());
				}
			}

			void CContentSizeControl::postLayout()
			{
				if (m_horizontalFit == WrapChildren &&
					m_verticalFit == WrapChildren)
				{
					sizeToChildren(true, true);
				}
				else if (m_horizontalFit == WrapChildren)
				{
					sizeToChildren(true, false);
				}
				else if (m_verticalFit == WrapChildren)
				{
					sizeToChildren(false, true);
				}
			}
		}
	}
}
