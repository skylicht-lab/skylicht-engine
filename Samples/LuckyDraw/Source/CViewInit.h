#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"
#include "Emscripten/CGetFileURL.h"

class CViewInit :
	public CView,
	public IEventReceiver
{
public:
	enum EInitState
	{
		DownloadBundles,
		InitScene,
		Error,
		Finished
	};

protected:
	CGetFileURL *m_getFile;

	EInitState m_initState;
	unsigned int m_downloaded;

	CGlyphFont *m_largeFont;

	CGameObject *m_canvasObject;
	CGUIText *m_guiText;
	float m_blinkTime;

protected:
	io::path getBuiltInPath(const char *name);

public:
	CViewInit();

	virtual ~CViewInit();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual bool OnEvent(const SEvent& event);

protected:

	void initScene();
};
