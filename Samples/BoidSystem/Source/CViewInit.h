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

	std::vector<CGameObject*> m_allObjects;

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

	void initEnviroment(CZone* zone, float& envMin, float& envMax);

	void initCrowdByAnimTexture(CZone* zone, float envMin, float envMax, std::vector<CAnimationClip*>& clips, int fps);

	void initSkinnedCrowd(CGameObject* crowd, CEntityPrefab* meshPrefab, core::matrix4* transforms, u32 w, u32 h, std::map<std::string, int>& bones);

	void initScene();
};
