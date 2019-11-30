#ifndef _TOUCH_IDENTIFY_H_
#define _TOUCH_IDENTIFY_H_

#define MAX_MULTITOUCH	10

namespace Skylicht
{

class CTouchIdentify
{
public:
	enum ETouchIdentify
	{
		Nothing = 0,
		TouchOnControl,
		TouchOnScreen,
	};

	struct STouchIdentity
	{
		int				TouchID;
		ETouchIdentify	Identitfy;
		void			*Data;
	};

protected:
	STouchIdentity	m_touchIdentity[MAX_MULTITOUCH];

public:
	CTouchIdentify();
	virtual ~CTouchIdentify();
	
	// touchRelease
	void touchPress(int pos, int touchID);
	
	// touchRelease
	void touchRelease(int pos, int touchID);

	// setTouchIdentify
	void setTouchIdentify(int touchID, CTouchIdentify::ETouchIdentify identify, void *data = NULL);

	// getTouchIdentify
	CTouchIdentify::ETouchIdentify getTouchIdentify(int touchID);
};

}

#endif