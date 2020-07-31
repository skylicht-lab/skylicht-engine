#pragma once

#include "IApplicationEventReceiver.h"
#include "Thread/IThread.h"

#include "UnwrapUV/CUnwrapUV.h"

// #define LIGHTMAP_SPONZA

class SampleLightmapUV : 
	public IApplicationEventReceiver,
	public SkylichtSystem::IThreadCallback
{
private:
	CScene *m_scene;
	
	CForwardRP *m_forwardRP;
	CCamera *m_camera;
	CCamera *m_guiCamera;

	Lightmapper::CUnwrapUV m_unwrap;

	CEntityPrefab *m_model;
	CRenderMesh *m_renderMesh;
	std::vector<CRenderMeshData*> m_allRenderers;

	ITexture *m_UVChartsTexture;

	SkylichtSystem::IThread *m_thread;

	bool m_threadFinish;
	bool m_initMeshUV;

	CGlyphFont *m_font;
	CGameObject *m_guiObject;
	CGUIText *m_textInfo;
public:
	SampleLightmapUV();
	virtual ~SampleLightmapUV();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();

	// SkylichtSystem::IThreadCallback implement
	virtual bool enableThreadLoop()
	{
		return false;
	}

	virtual void runThread();

	virtual void updateThread();

	void updateMeshUV();
};