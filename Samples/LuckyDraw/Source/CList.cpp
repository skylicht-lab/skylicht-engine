#include "pch.h"
#include "SkylichtEngine.h"
#include "CList.h"

CList::CList(CGUIElement* element) :
	m_element(element),
	m_basePosition(0.0f),
	m_offset(0.0f),
	m_targetOffset(0.0f),
	m_minOffset(0.0f),
	m_maxOffset(0.0f),
	m_drag(0.0f),
	m_dragSpeed(0.0f),
	m_mousePress(0.0f),
	m_mousePosY(0.0f),
	m_sendSelectEvent(false)
{
	m_mask = element->getCanvas()->createMask(element, element->getRect());

	CEventManager::getInstance()->registerEvent("CButton", this);
}

CList::~CList()
{
	clearAllItem();

	CEventManager::getInstance()->unRegisterEvent(this);
}

bool CList::OnEvent(const SEvent& gameEvent)
{
	if (gameEvent.EventType == EET_MOUSE_INPUT_EVENT)
	{
		// check mouse over on button
		// get current canvas
		CCanvas* canvas = m_element->getCanvas();

		// get last camera, that render this button
		CCamera* camera = canvas->getRenderCamera();
		if (camera == NULL)
			return false;

		const core::matrix4& world = canvas->getRenderWorldTransform();
		const core::matrix4& elementTranform = m_element->getAbsoluteTransform();

		// real world transform
		core::matrix4 worldElementTransform = world * elementTranform;

		core::rectf r = m_element->getRect();

		core::vector3df p[4];
		core::vector3df p2d[4];

		p[0].set(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, 0.0f);
		p[1].set(r.LowerRightCorner.X, r.UpperLeftCorner.Y, 0.0f);
		p[2].set(r.UpperLeftCorner.X, r.LowerRightCorner.Y, 0.0f);
		p[3].set(r.LowerRightCorner.X, r.LowerRightCorner.Y, 0.0f);

		for (int i = 0; i < 4; i++)
		{
			// get real 3d position
			worldElementTransform.transformVect(p[i]);

			// project to 2d screen
			CProjective::getScreenCoordinatesFrom3DPosition(camera, p[i], p2d[i].X, p2d[i].Y);
			p2d[i].Z = 0.0f;
		}

		// hit test
		core::triangle3df t1(p2d[0], p2d[1], p2d[2]);
		core::triangle3df t2(p2d[2], p2d[1], p2d[3]);
		core::vector3df mousePos(
			(f32)gameEvent.MouseInput.X,
			(f32)gameEvent.MouseInput.Y,
			0.0f
		);

		bool touchOver = false;
		if (t1.isPointInside(mousePos) == true || t2.isPointInside(mousePos) == true)
			touchOver = true;

		if (gameEvent.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN && touchOver)
		{
			m_mousePress = true;
			m_drag = 0.0f;
			m_mousePosY = gameEvent.MouseInput.Y;
			m_mousePressPosY = m_mousePosY;
			m_sendSelectEvent = false;
			return true;
		}
		else if (gameEvent.MouseInput.Event == EMIE_MOUSE_MOVED && m_mousePress)
		{
			// drag
			m_drag = gameEvent.MouseInput.Y - m_mousePressPosY;
			m_dragSpeed = gameEvent.MouseInput.Y - m_mousePosY;
			m_mousePosY = gameEvent.MouseInput.Y;

			// do not send select event
			if (fabsf(m_drag) > 30)
				m_sendSelectEvent = false;

			// limit drag
			float currentOffset = m_offset + m_drag;

			if (currentOffset < m_minOffset)
				m_drag = m_minOffset - m_offset;
			else if (currentOffset > m_maxOffset)
				m_drag = m_maxOffset - m_offset;

			return true;
		}
		else if (gameEvent.MouseInput.Event == EMIE_LMOUSE_LEFT_UP && m_mousePress)
		{
			m_mousePress = false;
			m_offset = m_offset + m_drag;
			m_drag = 0.0f;
			return true;
		}
	}

	return false;
}

void CList::clearAllItem()
{
	for (CGUIElement* item : m_items)
		item->remove();

	m_items.clear();
}

void CList::update()
{
	updateItemMovement();
	updateItemPosition();
}

void CList::updateItemPosition()
{
	float x = 0.0f;
	float y = m_basePosition + m_offset + m_drag;

	for (CGUIElement* item : m_items)
	{
		item->setPosition(core::vector3df(x, y, 0.0f));
		y = y + item->getHeight();
	}
}

void CList::updateItemMovement()
{
	if (m_mousePress == true)
		return;

	float timestep = getTimeStep();

	int items = (int)m_items.size();

	float itemHeight = 0.0f;

	m_minOffset = 0.0f;
	m_maxOffset = 0.0f;

	if (items > 0)
	{
		itemHeight = m_items[0]->getHeight();

		int numRow = m_element->getHeight() / itemHeight;

		m_maxOffset = itemHeight * 0.5f;

		if (items < numRow)
			m_minOffset = -m_maxOffset;
		else
			m_minOffset = -(items - numRow) * itemHeight - m_maxOffset;
	}

	if (fabs(m_dragSpeed) > 0.2f)
	{
		float dec = fabs(m_dragSpeed) * core::min_<float>(1.0f, 0.006f * timestep);

		if (m_dragSpeed > 0)
			m_dragSpeed = m_dragSpeed - dec;
		else
			m_dragSpeed = m_dragSpeed + dec;

		if (fabs(m_dragSpeed) < dec)
			m_dragSpeed = 0.0f;

		m_offset = m_offset + m_dragSpeed;

		if (m_offset < m_minOffset)
		{
			m_offset = m_minOffset;
			m_dragSpeed = -m_dragSpeed * 0.5f;
		}
		else if (m_offset > m_maxOffset)
		{
			m_offset = m_maxOffset;
			m_dragSpeed = -m_dragSpeed * 0.5f;
		}

		calcTargetOffset();
	}
	else
	{
		// scroll to target offset
		float delta = (float)m_targetOffset - m_offset;
		float step = delta * 0.006f * timestep;

		if (fabs(step) < 0.1f)
			m_offset = (float)m_targetOffset;
		else
			m_offset = m_offset + step;
	}
}

void CList::calcTargetOffset()
{
	int items = (int)m_items.size();
	if (items == 0)
		m_targetOffset = 0.0f;
	else
	{
		float itemHeight = (float)m_items[0]->getHeight();

		int selectItem = m_offset / itemHeight;

		if (-m_offset - (-selectItem * itemHeight) > itemHeight * 0.5f)
			selectItem--;

		if (selectItem <= -items)
			selectItem = -(items - 1);

		m_targetOffset = selectItem * itemHeight;
	}
}