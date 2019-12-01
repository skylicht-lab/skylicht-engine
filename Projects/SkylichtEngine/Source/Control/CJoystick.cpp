#include "pch.h"
#include "CJoystick.h"

namespace Skylicht
{

CJoystick::CJoystick()
{
	m_support = false;
}

CJoystick::~CJoystick()
{
}

// update
void CJoystick::update()
{
	SEvent event;	

	// push game event
	for (int i = 0, n = (int)m_eventQueue.size(); i < n; i++)
	{
		event.EventType = irr::EET_GAMEPAD_INPUT_EVENT;
		
		if (m_eventQueue[i].IsAnalog == false)
		{
			event.GamepadEvent.IsAnalog = false;
			event.GamepadEvent.GamepadKey = m_eventQueue[i].Key;
			event.GamepadEvent.PressedDown = m_eventQueue[i].Press;		
			event.GamepadEvent.Gamepad = m_eventQueue[i].DeviceID;
		}
		else
		{
			event.GamepadEvent.IsAnalog = true;
			event.GamepadEvent.GamepadKey = 0;
			event.GamepadEvent.PressedDown = true;		
			event.GamepadEvent.Gamepad = m_eventQueue[i].DeviceID;
			
			event.GamepadEvent.AnalogVecX[0] = m_eventQueue[i].X[0];
			event.GamepadEvent.AnalogVecY[0] = m_eventQueue[i].Y[0];
			event.GamepadEvent.AnalogVecX[1] = m_eventQueue[i].X[1];
			event.GamepadEvent.AnalogVecY[1] = m_eventQueue[i].Y[1];					
		}

		getIrrlichtDevice()->postEventFromUser(event);
	}

	m_eventQueue.set_used(0);
}

// keyEvent
void CJoystick::keyEvent(int deviceID, int keyID, bool pressDown)
{
	m_eventQueue.push_back(SInput());
	
	SInput &input = m_eventQueue.getLast();
	input.DeviceID = deviceID;
	input.Key = keyID;
	input.Press = pressDown;

	input.IsAnalog = false;
	input.X[0] = 0.0f;
	input.Y[0] = 0.0f;
	input.X[1] = 0.0f;
	input.Y[1] = 0.0f;
}

// analogEvent
void CJoystick::analogEvent(int deviceID, int ID, float x, float y)
{
	m_eventQueue.push_back(SInput());
	
	SInput &input = m_eventQueue.getLast();
	input.DeviceID = deviceID;
	input.Key = 0;
	input.Press = true;

	if (ID < 0 || ID > 1)
		ID = 0;

	input.IsAnalog = true;
	input.X[ID] = x;
	input.Y[ID] = y;
}

}