#pragma once

class CScroller;

class IScrollerCallback
{
public:
	virtual CGUIElement* createScrollElement(CScroller *scroller, CGUIElement *parent, const core::rectf& itemRect) = 0;

	virtual void updateScrollElement(CScroller *scroller, CGUIElement *item, int itemID) = 0;
};

class CScroller
{
protected:
	CGUIElement *m_element;
	std::vector<CGUIElement*> m_items;

	IScrollerCallback *m_callback;

	float m_startOffset;
	float m_absoluteOffset;
	float m_offset;
	float m_itemSize;

public:
	CScroller(CGUIElement *element, float itemSize, IScrollerCallback *callback);

	virtual ~CScroller();

	void update();

	const core::rectf& getRect()
	{
		return m_element->getRect();
	}

	inline void setStartOffset(float offset)
	{
		m_startOffset = offset;
	}

	inline float getStartOffset()
	{
		return m_startOffset;
	}

	inline void setOffset(float offset)
	{
		m_absoluteOffset = offset;
	}

	inline float getOffset()
	{
		return m_absoluteOffset;
	}

	inline float getItemSize()
	{
		return m_itemSize;
	}

protected:

	void updateItemPosition();

};