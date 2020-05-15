#pragma once

class CScroller
{
protected:
	CGUIElement *m_element;

public:
	CScroller(CGUIElement *element);

	virtual ~CScroller();
};