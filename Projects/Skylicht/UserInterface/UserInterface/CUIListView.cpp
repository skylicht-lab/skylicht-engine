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

#include "CUIEventManager.h"

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
			m_pointerX(0.0f),
			m_pointerY(0.0f),
			m_lastPointerX(0.0f),
			m_lastPointerY(0.0f),
			m_speed(0.0f),
			m_maxOffset(0.0f),
			m_springOffset(0.0f),
			m_itemSpacing(0.0f)
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
			if (m_element && m_itemSerializable)
			{
				CGUIElement* item = CGUIImporter::importGUI(m_element->getCanvas(), m_element, m_itemSerializable);
				item->setMask(m_mask);
				m_items.push_back(item);
				return item;
			}
			return NULL;
		}

		CGUIElement* CUIListView::addItem(CObjectSerializable* data)
		{
			if (m_element)
			{
				CGUIElement* item = CGUIImporter::importGUI(m_element->getCanvas(), m_element, data);
				item->setMask(m_mask);
				m_items.push_back(item);
				return item;
			}
			return NULL;
		}

		void CUIListView::enableMask(bool b)
		{
			if (m_mask)
			{
				m_mask->setVisible(b);
			}
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

		void CUIListView::clear()
		{
			auto i = m_items.begin(), end = m_items.end();
			while (i != end)
			{
				CGUIElement* item = (*i);
				m_container->removeChildsByGUI(item);
				item->remove();
				++i;
			}
			m_items.clear();
		}

		CGUIElement* CUIListView::getItem(int id)
		{
			if (id < 0 || id >= m_items.size())
				return NULL;
			return m_items[id];
		}

		void CUIListView::onPointerDown(float pointerX, float pointerY)
		{
			CUIBase::onPointerDown(pointerX, pointerY);

			CUIEventManager::getInstance()->setCapture(this);

			m_lastPointerX = pointerX;
			m_lastPointerY = pointerY;
			m_pointerX = pointerX;
			m_pointerY = pointerY;
		}

		void CUIListView::onPointerUp(float pointerX, float pointerY)
		{
			CUIBase::onPointerUp(pointerX, pointerY);

			CUIEventManager::getInstance()->setCapture(NULL);
		}

		void CUIListView::onPointerMove(float pointerX, float pointerY)
		{
			CUIBase::onPointerMove(pointerX, pointerY);

			m_lastPointerX = m_pointerX;
			m_lastPointerY = m_pointerY;
			m_pointerX = pointerX;
			m_pointerY = pointerY;
		}

		void CUIListView::update()
		{
			updateLimitOffset();
			updateItemPosition();
			updateItemMovement();
		}

		void CUIListView::updateLimitOffset()
		{
			m_springOffset = 0.0f;
			m_maxOffset = 0.0f;

			if (m_items.size() > 0)
			{
				if (m_vertical)
				{
					float height = m_element->getHeight();
					m_springOffset = height * 0.2f;

					for (CGUIElement* item : m_items)
						m_maxOffset = m_maxOffset + item->getHeight() + m_itemSpacing;

					m_maxOffset = m_maxOffset - height;
					if (m_maxOffset < 0.0f)
						m_maxOffset = 0.0f;
				}
				else
				{
					float width = m_element->getWidth();
					m_springOffset = width * 0.2f;

					for (CGUIElement* item : m_items)
						m_maxOffset = m_maxOffset + item->getWidth() + m_itemSpacing;

					m_maxOffset = m_maxOffset - width;
					if (m_maxOffset < 0.0f)
						m_maxOffset = 0.0f;
				}
			}
		}

		void CUIListView::updateItemPosition()
		{
			float x = 0.0f, y = 0.0f;

			if (m_vertical)
			{
				x = 0.0f;
				y = m_offset;
			}
			else
			{
				x = m_offset;
				y = 0.0f;
			}

			for (CGUIElement* item : m_items)
			{
				item->setPosition(core::vector3df(x, y, 0.0f));

				if (m_vertical)
					y = y + item->getHeight() + m_itemSpacing;
				else
					x = x + item->getWidth() + m_itemSpacing;
			}
		}

		void CUIListView::updateItemMovement()
		{
			if (m_isPointerDown)
			{
				const core::vector3df& scale = m_element->getCanvas()->getRootScale();

				if (m_vertical)
					m_speed = (m_pointerY - m_lastPointerY) / scale.Y;
				else
					m_speed = (m_pointerX - m_lastPointerX) / scale.X;

				updateStopSpeed();

				m_offset = m_offset + m_speed;

				m_lastPointerX = m_pointerX;
				m_lastPointerY = m_pointerY;
			}
			else
			{
				updateInertia();
			}
		}

		void CUIListView::updateStopSpeed()
		{
			if (m_springOffset > 0)
			{
				if (m_offset > 0.0f && m_speed > 0.0f)
				{
					float d = 1.0f - (m_offset / m_springOffset);
					if (d < 0.0f)
						d = 0.0f;
					m_speed = m_speed * d;
				}
				else if (m_offset < -m_maxOffset && m_speed < 0.0f)
				{
					float f = -(m_offset + m_maxOffset);
					float d = 1.0f - (f / m_springOffset);
					if (d < 0.0f)
						d = 0.0f;
					m_speed = m_speed * d;
				}
			}
		}

		void CUIListView::updateInertia()
		{
			float ts = getNonScaledTimestep();

			if (fabs(m_speed) > 0.2f)
			{
				float dec = fabs(m_speed) * core::min_<float>(0.9f, 0.01f * ts);

				if (m_speed > 0)
					m_speed = m_speed - dec;
				else
					m_speed = m_speed + dec;

				if (fabs(m_speed) < dec)
					m_speed = 0.0f;

				m_offset = m_offset + m_speed;

				// note: m_offset will scroll from 0 to -maxOffset;

				if (m_offset > m_springOffset || m_offset < -(m_maxOffset + m_springOffset))
					updateStopSpeed();
			}
			else
			{
				float dec = core::min_<float>(0.9f, 0.005f * ts);

				if (m_offset > 0.0f)
				{
					// scroll to 0.0f
					m_offset = m_offset - m_offset * dec;
					if (m_offset < dec)
						m_offset = 0.0f;
				}
				else if (m_offset < -m_maxOffset)
				{
					// scroll to -maxOffset
					m_offset = m_offset - (m_offset + m_maxOffset) * dec;
					if (m_offset > (-m_maxOffset - dec))
						m_offset = -m_maxOffset;
				}
			}
		}
	}
}