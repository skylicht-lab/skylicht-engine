#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"

#include "Lightmapper/CLightmapper.h"
#include "Lightmapper/CSH9.h"
#include "Rasterisation/CRasterisation.h"

#define MAX_LIGHTMAP_ATLAS 40

#define LIGHTMAP_SPONZA

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

	u32 m_numRenderers;
	u32 m_numIndices;
	u32 m_numVertices;

	u32 m_timeBeginBake;
	u32 m_timeSpentFromLastSave;

	static u32 s_numLightBounce;

	u32 m_lightBounce;

	u32 m_currentPass;
	u32 m_currentMB;

	u32 m_currentTris;
	u32 m_lastTris;

	u32 m_lightmapSize;

	core::vector2di m_pixel;

	Lightmapper::CRasterisation *m_lmRasterize[MAX_LIGHTMAP_ATLAS];
	int m_numberRasterize;

	Lightmapper::CRasterisation *m_currentRasterisation;

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

protected:

	Lightmapper::CRasterisation* createGetLightmapRasterisation(int index);
	int getRasterisationIndex(Lightmapper::CRasterisation *raster);

public:
	void saveProgress();
	void loadProgress();
};