/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CUIContainer.h"
#include "CUIListView.h"
#include "Graphics2D/CGUIImporter.h"
#include "Graphics2D/CGUIExporter.h"

namespace Skylicht
{
	namespace UI
	{
		CUIListView::CUIListView(CUIContainer* container, CGUIElement* element, CGUIElement* baseItem) :
			CUIBase(container, element),
			m_vertical(true),
			m_mask(NULL),
			m_baseItem(baseItem),
			m_itemSerializable(NULL),
			m_offset(0.0f),
			m_drag(0.0f),
			m_pointerX(0.0f),
			m_pointerY(0.0f),
			m_pointerDownX(0.0f),
			m_pointerDownY(0.0f)
		{
			if (m_element)
			{
				m_mask = m_element->getCanvas()->createMask(m_element, m_element->getRect());
				m_mask->setDock(EGUIDock::DockFill);

				if (m_baseItem)
				{
					m_itemSerializable = CGUIExporter::createSerializable(m_baseItem);
					m_baseItem->setVisible(false);
				}
			}
		}

		CUIListView::~CUIListView()
		{
			if (m_itemSerializable)
				delete m_itemSerializable;
		}

		CGUIElement* CUIListView::addItem()
		{
			if (m_itemSerializable)
			{
				CGUIElement* item = CGUIImporter::importGUI(m_element->getCanvas(), m_element, m_itemSerializable);
				item->setMask(m_mask);
				m_items.push_back(item);
				return item;
			}
			return NULL;
		}

		bool CUIListView::removeItem(CGUIElement* item)
		{
			auto i = m_items.begin(), end = m_items.end();
			while (i != end)
			{
				if ((*i) == item)
				{
					m_items.erase(i);
					m_container->removeChildsByGUI(item);
					item->remove();
					return true;
				}
				++i;
			}
			return false;
		}

		void CUIListView::onPointerDown(float pointerX, float pointerY)
		{
			CUIBase::onPointerDown(pointerX, pointerY);

			m_pointerDownX = pointerX;
			m_pointerDownY = pointerY;
		}

		void CUIListView::onPointerUp(float pointerX, float pointerY)
		{
			CUIBase::onPointerUp(pointerX, pointerY);
			m_offset = m_offset + m_drag;
			m_drag = 0.0f;
		}

		void CUIListView::onPointerMove(float pointerX, float pointerY)
		{
			CUIBase::onPointerMove(pointerX, pointerY);

			m_pointerX = pointerX;
			m_pointerY = pointerY;
		}

		void CUIListView::update()
		{
			if (m_vertical)
			{
				updateItemPosition();
				updateItemMovement();
			}
			else
				updateItemPositionHorizontal();
		}

		void CUIListView::updateItemPosition()
		{
			float x = 0.0f;
			float y = m_offset + m_drag;

			for (CGUIElement* item : m_items)
			{
				item->setPosition(core::vector3df(x, y, 0.0f));
				y = y + item->getHeight();
			}
		}

		void CUIListView::updateItemMovement()
		{
			if (m_isPointerDown)
			{
				m_drag = m_pointerY - m_pointerDownY;
			}
			else
			{

			}
		}

		void CUIListView::updateItemPositionHorizontal()
		{

		}
	}
}