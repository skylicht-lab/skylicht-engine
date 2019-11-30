// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "pch.h"
#include "CTouchManager.h"
#include "Graphics/CGraphics.h"

namespace Skylicht
{

CTouchManager::CTouchManager()
{
	m_mutex = SkylichtSystem::IMutex::createMutex();

	resetTouch();
}

CTouchManager::~CTouchManager()
{
	if (m_mutex)
	{
		delete m_mutex;
		m_mutex = NULL;
	}
}

// resetTouch
void CTouchManager::resetTouch()
{	
	// reset to touchEvent
	for ( int i = 0; i < MAX_MULTITOUCH; i++ )
	{
		m_touch[i].touchEvent	= CTouchManager::TouchNone;
		m_touch[i].x			= 0;
		m_touch[i].y			= 0;			
		m_touch[i].touchID		= -1;		
	}	
}

// update
// reset touch & send event to irrlicht device
void CTouchManager::update()
{
	// lock mutex
	SkylichtSystem::SScopeMutex lockScope(m_mutex);

	int n = m_touchQueue.size();
	if (n == 0)
	{
		updateEvent();
	}
	else
	{		
		for (int i = 0; i < n; i++)
		{
			STouchStatus& t = m_touchQueue[i];
			
			updateTouch(t.touchEvent, t.x, t.y, t.touchID);
			updateEvent();
		}
		m_touchQueue.set_used(0);		
	}
}

// touchEvent
// update touch event
void CTouchManager::touchEvent(ETouchEvent touchEvent, int x, int y, long id)
{
	// lock mutex
	SkylichtSystem::SScopeMutex lockScope(m_mutex);

	// todo add event to queue
	STouchStatus t;
	t.touchEvent = touchEvent;
	t.touchID = id;
	t.x = x;
	t.y = y;
	m_touchQueue.push_back(t);	
}

// updateQueue
void CTouchManager::updateTouch(ETouchEvent touchEvent, int x, int y, long id)
{
	// scale touch
	float scale = CGraphics::getInstance()->getScale();
	x = (int)((float)x * scale);
	y = (int)((float)y * scale);

	//if (touchEvent != CTouchManager::TouchMove)
	//{
	//	char msg[1024] = {0};
	//	sprintf(msg, "touchEvent %d %d %d", (int)touchEvent, x, y, id);
	//	os::Printer::log(msg);
	//}

	for (int i = 0; i < MAX_MULTITOUCH; i++)
	{
		if (m_touch[i].touchID == id )
		{
			// we only update touch action when it moving
			// Make only 1 time touch down is called
			if (touchEvent == CTouchManager::TouchDown)
			{
			}
			else if (touchEvent == CTouchManager::TouchMove || touchEvent == CTouchManager::TouchUp)
			{
				m_touch[i].x = x;
				m_touch[i].y = y;
				m_touch[i].touchEvent = touchEvent;				
			}			
			return;
		}
	}

	// add new touch
	for (int i = 0; i < MAX_MULTITOUCH; i++)
	{
		if (m_touch[i].touchEvent == CTouchManager::TouchNone && touchEvent == CTouchManager::TouchDown)
		{
			m_touch[i].touchEvent	= CTouchManager::TouchDown;
			m_touch[i].x			= x;
			m_touch[i].y			= y;
			m_touch[i].touchID		= id;
			return;
		}
	}	
}

// updateEvent
void CTouchManager::updateEvent()
{
	irr::SEvent		event;
	IrrlichtDevice *device = getIrrlichtDevice();

	// notify event to irrlicht device
	for (int i = 0; i < MAX_MULTITOUCH; i++)
	{
		// set touch variable
		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.ID = (int)m_touch[i].touchID;
		event.MouseInput.X = m_touch[i].x;
		event.MouseInput.Y = m_touch[i].y;
		event.MouseInput.Shift = false;
		event.MouseInput.Control = false;

		if (m_touch[i].touchEvent == CTouchManager::TouchDown)
		{
			// identity touch
			m_touchIdentify.touchPress(i, (int)m_touch[i].touchID);

			event.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
			device->postEventFromUser(event); 
		}
		else if (m_touch[i].touchEvent == CTouchManager::TouchMove)
		{			
			event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
			device->postEventFromUser(event);
		}
		else if (m_touch[i].touchEvent == CTouchManager::TouchUp)
		{			
			// identity touch
			m_touchIdentify.touchRelease(i, (int)m_touch[i].touchID);

			event.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
			device->postEventFromUser(event);			
		}
	}

	// reset to touchEvent
	for ( int i = 0; i < MAX_MULTITOUCH; i++ )
	{
		if (m_touch[i].touchEvent == CTouchManager::TouchUp)
		{
			m_touch[i].touchEvent	= CTouchManager::TouchNone;
			m_touch[i].x			= 0;
			m_touch[i].y			= 0;
			m_touch[i].touchID		= -1;
		}
        else if (m_touch[i].touchEvent == CTouchManager::TouchDown)
        {
            // switch to touch move event for next update
            m_touch[i].touchEvent = CTouchManager::TouchMove;
        }
	}
}

}