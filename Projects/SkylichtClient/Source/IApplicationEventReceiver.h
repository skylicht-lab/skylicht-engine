#ifndef _APPLICATION_EVENT_H_
#define _APPLICATION_EVENT_H_

class IApplicationEventReceiver
{
public:
	virtual void onUpdate() = 0;

	virtual void onRender() = 0;

	virtual void onPostRender() = 0;

	virtual void onResume() = 0;

	virtual void onPause() = 0;

	virtual void onQuitApp() = 0;
};

#endif