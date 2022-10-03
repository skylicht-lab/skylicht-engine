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
#include "CThumbnailView.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CThumbnailView::CThumbnailView(CBase* parent, float itemWidth) :
				CBase(parent),
				m_itemWidth(itemWidth)
			{
				m_view = new CScrollControl(this);
				m_view->dock(EPosition::Fill);
			}

			CThumbnailView::~CThumbnailView()
			{

			}

			void CThumbnailView::layout()
			{
				CBase::layout();
			}

			void CThumbnailView::postLayout()
			{
				CBase::postLayout();
			}

			void CThumbnailView::unSelectAll()
			{

			}

			bool CThumbnailView::onKeyUp(bool down)
			{
				return true;
			}

			bool CThumbnailView::onKeyDown(bool down)
			{
				return true;
			}

			bool CThumbnailView::onKeyHome(bool down)
			{
				return true;
			}

			bool CThumbnailView::onKeyEnd(bool down)
			{
				return true;
			}

			void CThumbnailView::onItemDown(CBase* base)
			{

			}
		}
	}
}