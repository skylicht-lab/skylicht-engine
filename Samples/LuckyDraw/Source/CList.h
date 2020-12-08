#pragma once

class CList : public IEventReceiver
{
protected:
	CGUIElement* m_element;

	CGUIMask* m_mask;

	std::vector<CGUIElement*> m_items;

	float m_basePosition;

	float m_offset;

	float m_targetOffset;

	float m_minOffset;

	float m_maxOffset;

	float m_drag;

	float m_dragSpeed;

	float m_mousePressPosY;

	float m_mousePosY;

	bool m_mousePress;

	bool m_sendSelectEvent;

public:
	CList(CGUIElement* element);

	virtual ~CList();

	virtual bool OnEvent(const SEvent& event);

	void clearAllItem();

	void update();

	void setBasePosition(float p)
	{
		m_basePosition = p;
	}

	void updateItemPosition();

	void updateItemMovement();

	void calcTargetOffset();

	CGUIElement* getElement()
	{
		return m_element;
	}

	void addItem(CGUIElement* item)
	{
		item->setMask(m_mask);
		m_items.push_back(item);
	}

	void setVisible(bool b)
	{
		m_element->setVisible(b);
	}
};