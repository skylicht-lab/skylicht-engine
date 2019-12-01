#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include "Utils/CGameSingleton.h"

namespace Skylicht
{

#define MAX_JOYSTICK_DEVICE	2

class CJoystick : public CGameSingleton<CJoystick>
{
protected:
	bool m_support;

	struct SInput
	{
		int Key;
		int DeviceID;
		bool Press;

		bool IsAnalog;
		float X[2];
		float Y[2];
	};

	core::array<SInput>	m_eventQueue;

public:
	CJoystick();
	virtual ~CJoystick();

	// setSupport
	void setSupport(bool b)
	{
		m_support = b;
	}

	// isSupport
	bool isSupport()
	{
		return m_support;
	}

	// update
	void update();

	// keyEvent
	void keyEvent(int deviceID, int keyID, bool pressDown);

	// analogEvent
	void analogEvent(int deviceID, int ID, float x, float y);
};

}

#endif