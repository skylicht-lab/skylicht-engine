#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"

#include "Lightmapper/CLightmapper.h"
#include "Lightmapper/CSH9.h"
#include "Rasterisation/CRasterisation.h"

class CViewBakeLightmap : public CView
{
protected:
	std::vector<CRenderMesh*> m_renderMesh;

	std::vector<IMeshBuffer*> m_meshBuffers;
	core::array<core::matrix4> m_meshTransforms;

	CGameObject *m_bakeCameraObject;
	CGameObject *m_guiObject;
	CGlyphFont *m_font;
	CGUIText *m_textInfo;

	u32 m_timeBeginBake;

	static u32 s_numLightBounce;

	u32 m_lightBounce;

	u32 m_currentPass;
	u32 m_currentMB;

	u32 m_currentTris;
	u32 m_lastTris;

	core::vector2di m_pixel;

	Lightmapper::CRasterisation *m_lmRasterize;

	std::vector<Lightmapper::CSH9> m_out;

	core::vector3df m_bakePositions[MAX_NUM_THREAD];
	core::vector3df m_bakeNormals[MAX_NUM_THREAD];
	core::vector3df m_bakeTangents[MAX_NUM_THREAD];
	core::vector3df m_bakeBinormals[MAX_NUM_THREAD];

public:

	CViewBakeLightmap();

	virtual ~CViewBakeLightmap();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();
};