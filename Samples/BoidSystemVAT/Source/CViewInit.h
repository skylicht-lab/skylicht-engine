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

	void initCrowdByVertexTexture(CZone* zone, float envMin, float envMax, std::vector<CAnimationClip*>& clips, int fps);

	void initVATCrowd(CGameObject* crowd, CEntityPrefab* meshPrefab, std::vector<CAnimationClip*>& clips, int fps);

	void initScene();
};
