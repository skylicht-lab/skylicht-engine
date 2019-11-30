#ifndef _TOUCHMANAGER_H_
#define _TOUCHMANAGER_H_

#include "Thread/IMutex.h"
#include "Utils/CGameSingleton.h"
#include "CTouchIdentify.h"

namespace Skylicht
{

class CTouchManager: public CGameSingleton<CTouchManager>
{
public:
	enum ETouchEvent
	{
		TouchNone = 0,
		TouchDown,
		TouchMove,
		TouchUp,		
	};

	struct STouchStatus
	{
		ETouchEvent	touchEvent;		
		int			x;
		int			y;
		long		touchID;
	};

protected:
	core::array<STouchStatus>	m_touchQueue;

	STouchStatus	m_touch[MAX_MULTITOUCH];
	CTouchIdentify	m_touchIdentify;

	SkylichtSystem::IMutex	*m_mutex;	

public:
	CTouchManager();
	virtual ~CTouchManager();

	// resetTouch
	void resetTouch();

	// update
	// reset touch & send event to irrlicht device
	void update();

	// touchEvent
	// update touch event
	void touchEvent(ETouchEvent touchEvent, int x, int y, long id);

	// getTouchIdentify
	CTouchIdentify* getTouchIdentify()
	{
		return &m_touchIdentify;
	}

	// setTouchIdentify
	void setTouchIdentify(int touchID, CTouchIdentify::ETouchIdentify identify, void *data = NULL)
	{
		m_touchIdentify.setTouchIdentify(touchID, identify, data);
	}

	// getTouchIdentify
	CTouchIdentify::ETouchIdentify getTouchIdentify(int touchID)
	{
		return m_touchIdentify.getTouchIdentify(touchID);
	}
	
	// updateTouch
	void updateTouch(ETouchEvent touchEvent, int x, int y, long id);

	// updateEvent
	void updateEvent();

};

}

#endif