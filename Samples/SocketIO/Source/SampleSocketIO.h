#pragma once

#include "IApplicationEventReceiver.h"

#include "SocketIO/CSocketIO.h"
using namespace Skylicht::Network;

class SampleSocketIO :
	public IApplicationEventReceiver
{
private:
	CScene* m_scene;
	CCamera* m_guiCamera;
	CCamera* m_camera;

	CGlyphFont* m_largeFont;

	CForwardRP* m_forwardRP;

	CSocketIO* m_io;

	std::vector<std::string> m_logs;
	bool m_scrollToBottom;
	bool m_autoScroll;
	char m_inputBuffer[256];

public:
	SampleSocketIO();
	virtual ~SampleSocketIO();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onResize(int w, int h);

	virtual void onInitApp();

	virtual void onQuitApp();
};
