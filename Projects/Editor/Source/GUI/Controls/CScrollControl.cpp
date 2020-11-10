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
#include "CScrollControl.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CScrollControl::CScrollControl(CBase *parent) :
				CBase(parent),
				m_canScrollV(true),
				m_canScrollH(true)
			{
				setPadding(SPadding(0.0f, 0.0f, -1.0f, 0.0f));
				m_vertical = new CScrollBar(this, false);
				m_vertical->dock(EPosition::Right);
				m_vertical->OnBarMoved = BIND_LISTENER(&CScrollControl::onScrollBarV, this);

				m_horizontal = new CScrollBar(this, true);
				m_horizontal->dock(EPosition::Bottom);
				m_vertical->OnBarMoved = BIND_LISTENER(&CScrollControl::onScrollBarH, this);

				m_innerPanel = new CBase(this);
				m_innerPanel->setPos(0.0f, 0.0f);
				m_innerPanel->sendToBack();
				m_innerPanel->enableClip(true);
			}

			CScrollControl::~CScrollControl()
			{

			}

			void CScrollControl::layout()
			{
				updateScrollBar();
				CBase::layout();
			}

			void CScrollControl::onChildBoundsChanged(const SRect& oldChildBounds, CBase* child)
			{
				updateScrollBar();
				invalidate();
			}

			void CScrollControl::onScrollBarV(CBase *base)
			{
				invalidate();
			}

			void CScrollControl::onScrollBarH(CBase *base)
			{
				invalidate();
			}

			void CScrollControl::showScrollBar(bool h, bool v)
			{
				m_canScrollV = v;
				m_canScrollH = h;

				m_vertical->setHidden(!v);
				m_horizontal->setHidden(!h);
			}

			void CScrollControl::updateScrollBar()
			{
				if (m_innerPanel == NULL)
					return;

				float childrenWidth = 0.0f;
				float childrenHeight = 0.0f;

				float w = width() - (m_vertical->isHidden() ? 0 : m_vertical->width());
				float h = height() - (m_horizontal->isHidden() ? 0 : m_horizontal->height());


				// Get the max size of all our children together
				for (CBase* child : m_innerPanel->Children)
				{
					childrenWidth = core::max_(childrenWidth, child->right());
					childrenHeight = core::max_(childrenHeight, child->bottom());
				}

				m_innerPanel->setSize(core::max_(w, childrenWidth), core::max_(h, childrenHeight));

				m_vertical->setContentSize(m_innerPanel->height());
				m_vertical->setViewableContentSize(h);

				m_horizontal->setContentSize(m_innerPanel->width());
				m_horizontal->setViewableContentSize(w);

				float newInnerPanelPosX = 0.0f;
				float newInnerPanelPosY = 0.0f;

				if (m_canScrollV && !m_vertical->isHidden())
				{
					newInnerPanelPosY = -(m_innerPanel->height() - height() + m_horizontal->height()) * m_vertical->getScroll();
				}

				if (m_canScrollH && !m_horizontal->isHidden())
				{
					newInnerPanelPosX = -(m_innerPanel->width() - width() + m_vertical->width()) * m_horizontal->getScroll();
				}

				m_innerPanel->setPos(newInnerPanelPosX, newInnerPanelPosY);
			}
		}
	}
}