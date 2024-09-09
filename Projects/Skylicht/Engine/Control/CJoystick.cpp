/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CJoystick.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CJoystick);

	CJoystick::CJoystick() :
		m_support(false)
	{
	}

	CJoystick::~CJoystick()
	{
	}

	void CJoystick::update()
	{
		SEvent event;

		// push game event
		for (u32 i = 0, n = m_eventQueue.size(); i < n; i++)
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

	void CJoystick::keyEvent(int deviceID, int keyID, bool pressDown)
	{
		m_eventQueue.push_back(SInput());

		SInput& input = m_eventQueue.getLast();
		input.DeviceID = deviceID;
		input.Key = keyID;
		input.Press = pressDown;

		input.IsAnalog = false;
		input.X[0] = 0.0f;
		input.Y[0] = 0.0f;
		input.X[1] = 0.0f;
		input.Y[1] = 0.0f;
	}

	void CJoystick::analogEvent(int deviceID, int ID, float x, float y)
	{
		m_eventQueue.push_back(SInput());

		SInput& input = m_eventQueue.getLast();
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