#ifndef _VIEW_INIT_H_
#define _VIEW_INIT_H_

#include "ViewManager/CView.h"
#include "Emscripten/CGetFileURL.h"

class CViewInit : public CView
{
public:
	enum EInitState
	{
		InitEngine,
		DownloadBundles,
		InitScene,
		Error,
		Finished
	};

protected:
	CGetFileURL *m_getFile;

	EInitState m_initState;

protected:
	io::path getBuiltInPath(const char *name);

public:
	CViewInit();

	virtual ~CViewInit();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

protected:

	void initScene();
};

#endif