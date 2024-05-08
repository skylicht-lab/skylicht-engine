#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"
#include "Emscripten/CGetFileURL.h"

class CViewInit : public CView
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
	CGetFileURL* m_getFile;

	bool m_bakeSHLighting;

	EInitState m_initState;
	unsigned int m_downloaded;

	CGameObject* m_guiObject;
	CGUIText* m_textInfo;
	CGlyphFont* m_font;

	CGameObject* m_plane;
	CGameObject* m_crowd1;
	CGameObject* m_crowd2;

protected:
	io::path getBuiltInPath(const char* name);

public:
	CViewInit();

	virtual ~CViewInit();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

protected:

	void initScene();

	void initCrowd(CGameObject* crowd, CEntityPrefab* meshPrefab, core::matrix4* transforms, u32 w, u32 h, std::map<std::string, int>& bones);
};
