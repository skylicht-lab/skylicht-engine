#ifndef _CAPPLICATION_H_
#define _CAPPLICATION_H_

#include "pch.h"
#include "CBaseApp.h"

enum EAppEvent
{
	AppEventUpdate,
	AppEventRender,
	AppEventPostRender,
	AppEventPause,
	AppEventResume,
	AppEventBack,
	AppEventQuit,
	AppEventInit
};

class CApplication: public IEventReceiver, CBaseApp
{	
public:
	unsigned long	m_lastUpdateTime;
	int				m_fps;
	
	unsigned long	m_pauseTime;

protected:
	bool			m_resizeWin;

protected:
    int             m_width;
    int             m_height;

	bool			m_runGame;
	bool			m_showFPS;
	bool			m_show2DUI;

	float			m_tickSecond;
public:
	CApplication();	

	virtual bool OnEvent(const SEvent& event);	

	// getBaseApp
	// for multi-inherit
	CBaseApp* getBaseApp()
	{
		return (CBaseApp*) this;
	}

	// initApplication
	// init app
	void initApplication( IrrlichtDevice* device );	

	// destroyApplication
	// destroy app
	void destroyApplication();	

	// mainLoop
	// loop game
	void mainLoop();	

	// back
	int back();

    // pause
    void pause();
    
    // resume
    void resume(int showConnecting = 1);
    
    void resetTouch();

	// notifyResizeWin
	// notify change size of window
	void notifyResizeWin(int w, int h);

	//Using for Handle Tracking, LegionManager, ... Update
	void additionalUpdate(float dt);

	// updateTouch
	// action:
	//	+ 0:	TouchDown
	//	+ 1:	TouchUp
	//	+ 2:	TouchMove	
	void updateTouch(long touchID, int x, int y, int action);

	// updateJoystick
	// action:
	// + 0: Press
	// + 1: Release
	void updateJoystick(int deviceID, int key, int action);
	
	// updateAnalog
	void updateAnalog(int deviceID, int id, float x, float y);

	// updateAccelerometer
	// update motion sensor control
	void updateAccelerometer(float x, float y, float z);
	
	// setAccelerometerSupport
	void setAccelerometerSupport(bool b);
	void setAccelerometerEnable(bool b);
	bool isAccelerometerEnable();

	// setDeviceID
	void setDeviceID(const wchar_t* string);
	void setDeviceID(const char* string);

	// sendEventToAppReceiver
	void sendEventToAppReceiver(int eventID);
};


#endif