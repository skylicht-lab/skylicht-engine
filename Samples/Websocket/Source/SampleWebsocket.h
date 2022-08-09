#pragma once

#include "IApplicationEventReceiver.h"

#include "Websocket/CWebsocket.h"

class SampleWebsocket :
	public IApplicationEventReceiver,
	public IWebsocketCallback
{
private:
	CScene* m_scene;
	CCamera* m_guiCamera;
	CCamera* m_camera;

#if defined(USE_FREETYPE)	
	CGlyphFont* m_largeFont;
#endif

	CForwardRP* m_forwardRP;

	CWebsocket* m_websocket;

public:
	SampleWebsocket();
	virtual ~SampleWebsocket();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onResize(int w, int h);

	virtual void onInitApp();

	virtual void onQuitApp();

	virtual void onWebSocketMessage(const std::string& data);
};