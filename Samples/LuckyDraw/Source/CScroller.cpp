#include "pch.h"
#include "SkylichtEngine.h"
#include "CScroller.h"

CScroller::CScroller(CGUIElement *element, float itemSize, IScrollerCallback *callback) :
	m_element(element),
	m_callback(callback),
	m_startOffset(0.0f),
	m_absoluteOffset(0.0f),
	m_offset(0.0f),
	m_itemSize(itemSize)
{
	CCanvas *canvas = element->getCanvas();

	// scroll size
	const core::rectf& scrollerRect = element->getRect();
	float scrollHeight = scrollerRect.getHeight();
	float scrollWidth = scrollerRect.getWidth();

	// create mask
	CGUIMask *mask = canvas->createMask(element, scrollerRect);

	// compute number of items in scroll
	float f = 1.0f + scrollHeight / (float)itemSize;
	int numItem = (int)f;
	if ((int)f < f)
		numItem++;

	// create scroll items
	core::rectf itemRect(0.0f, 0.0f, scrollWidth, itemSize);
	for (int i = 0; i < numItem; i++)
	{
		CGUIElement *item = m_callback->createScrollElement(this, m_element, itemRect);
		item->setMask(mask);
		m_items.push_back(item);
	}

	// update position
	updateItemPosition();
}

CScroller::~CScroller()
{

}

void CScroller::update()
{
	updateItemPosition();
}

void CScroller::updateItemPosition()
{
	int numItem = (int)m_items.size();

	if (numItem == 0)
		return;

	float offset = m_startOffset + m_absoluteOffset;

	float scrollSize = m_items.size() * m_itemSize;
	int loop = (int)(-offset / scrollSize);

	// offset to move item
	m_offset = fmodf(offset, scrollSize);

	// todo sort item
	std::vector<CGUIElement*>::iterator i = m_items.begin(), end = m_items.end();

	float x = 0.0f;
	float y = m_offset;
	float scrollHeight = m_element->getHeight();

	int id = 0;

	while (i != end)
	{
		CGUIElement *item = (*i);

		int loopItem = loop;

		if (y + m_itemSize < 0.0f)
		{
			// hide top
			float move = y + numItem * m_itemSize;
			item->setPosition(core::vector3df(x, move, 0.0f));

			loopItem++;
		}
		else if (y > scrollHeight)
		{
			// hide bottom
			float move = y - numItem * m_itemSize;
			item->setPosition(core::vector3df(x, move, 0.0f));

			loopItem--;
		}
		else
		{
			// apply position
			item->setPosition(core::vector3df(x, y, 0.0f));
		}

		// callback
		if (m_callback)
			m_callback->updateScrollElement(this, item, loopItem * numItem + id);

		// next item
		y = y + m_itemSize;

		++i;
		id++;
	}

}