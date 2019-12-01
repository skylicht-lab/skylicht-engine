#ifndef _BASE_APP_
#define _BASE_APP_

#include "pch.h"
#include "IApplicationEventReceiver.h"

namespace Skylicht
{
	class CBaseApp
	{
	protected:
		IrrlichtDevice*			m_device;
		IVideoDriver*			m_driver;
		io::IFileSystem*		m_fileSystem;

		SExposedVideoData		m_videoData;

		float					m_timeStep;
		int						m_limitFPS;

		float					m_clearScreenTime;
		bool					m_enableRender;

		video::SColor			m_clearColor;
	public:

		CBaseApp();
		virtual ~CBaseApp();

		typedef std::pair<std::string, IEventReceiver*> eventType;
		std::vector<eventType> m_eventReceivers;

		typedef std::pair<std::string, IApplicationEventReceiver*> appEventType;
		std::vector<appEventType> m_appEventReceivers;

	public:

		// getDevice
		// get irr device
		inline IrrlichtDevice* getDevice()
		{
			return m_device;
		}

		// getDriver
		// get irr driver
		inline IVideoDriver* getDriver()
		{
			return m_driver;
		}

		// getFileSystem
		// return filesytem for read/write file
		inline io::IFileSystem* getFileSystem()
		{
			return m_fileSystem;
		}

		// setClearColor
		// clear color
		inline void setClearColor(const video::SColor& c)
		{
			m_clearColor = c;
		}

		// getVideoData
		// get viewData
		inline SExposedVideoData& getVideoData()
		{
			return m_videoData;
		}

		inline float getTimeStep()
		{
			return m_timeStep;
		}

		int getWidth();
		int getHeight();
		float getSizeRatio();
		bool isHD();
		bool isWideScreen();

		// clearScreenTime
		// set clear time
		inline void clearScreenTime(float f)
		{
			m_clearScreenTime = f;
		}

		inline void enableRender(bool b)
		{
			m_enableRender = b;
		}

		inline bool isEnableRender()
		{
			return m_enableRender;
		}

		// register event
		// register input event
		void registerEvent(std::string name, IEventReceiver *pEvent);

		// unRegisterEvent
		// unregister input event
		void unRegisterEvent(IEventReceiver *pEvent);

		// register event
		// register input event
		void registerAppEvent(std::string name, IApplicationEventReceiver *pEvent);

		// unRegisterEvent
		// unregister input event
		void unRegisterAppEvent(IApplicationEventReceiver *pEvent);

		// setLimitFPS
		void setLimitFPS(int fps)
		{
			m_limitFPS = fps;
		}

		// autoScaleUI
		void autoScaleUI();

		// alertError
		// show error msgbox
		virtual void alertError(wchar_t *lpString);

		// yesNoQuestion
		// show yes, no msgbox
		virtual bool yesNoQuestion(wchar_t *lpString);

		// setStatusText
		// set text on status bar
		virtual void setStatusText(int part, wchar_t *lpString);
	};

	extern CBaseApp* getApplication();
}

#endif