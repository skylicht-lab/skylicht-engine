#pragma once

class IScrollerCallback
{
public:
	virtual CGUIElement* createScrollElement(CGUIElement *parent, const core::rectf& itemRect) = 0;

	virtual void updateScrollElement(CGUIElement *item, int itemID) = 0;
};

class CScroller
{
protected:
	CGUIElement *m_element;
	std::vector<CGUIElement*> m_items;

	IScrollerCallback *m_callback;

	float m_absoluteOffset;
	float m_offset;
	float m_itemSize;

public:
	CScroller(CGUIElement *element, float itemHeight, IScrollerCallback *callback);

	virtual ~CScroller();

	void update();

	const core::rectf& getRect()
	{
		return m_element->getRect();
	}

	inline void setOffset(float offset)
	{
		m_absoluteOffset = offset;
	}

	inline float getOffset()
	{
		return m_absoluteOffset;
	}

protected:

	void updateItemPosition();

};