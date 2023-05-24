#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"

#include "Lightmapper/CLightmapper.h"
#include "Lightmapper/CSH9.h"
#include "Rasterisation/CRasterisation.h"

#define MAX_LIGHTMAP_ATLAS 10

// #define LIGHTMAP_SPONZA

class CViewBakeLightmap : public CView
{
protected:
	std::vector<CRenderMesh*> m_renderMesh;

	std::vector<IMeshBuffer*> m_meshBuffers;
	core::array<core::matrix4> m_meshTransforms;

	CGameObject* m_bakeCameraObject;
	CGameObject* m_guiObject;
	CGlyphFont* m_font;
	CGUIText* m_textInfo;

	int m_lightmapSize;
	int m_numRenderers;

public:

	CViewBakeLightmap();

	virtual ~CViewBakeLightmap();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

public:

	void gotoDemoView();
};