#pragma once

#include "ViewManager/CView.h"
#include "Lightmapper/CSH9.h"

using namespace Lightmapper;

class CViewBakeLightmap : public CView
{
public:
	struct SColorBuffer
	{
		core::array<SColor> Color;
		core::array<CSH9> SH;
	};

	static int s_numLightBound;

protected:
	CGameObject *m_guiObject;
	CGameObject *m_bakeCameraObject;

	CGUIText *m_textInfo;
	CGlyphFont *m_font;

	std::vector<CRenderMesh*> m_renderMesh;
	std::vector<IMeshBuffer*> m_allMeshBuffer;

	core::array<SColorBuffer*> m_colorBuffer;
	core::array<core::matrix4> m_meshTransforms;

	u32 m_currentMeshBuffer;
	u32 m_currentVertex;
	u32 m_totalVertexBaked;
	int m_lightBound;

	u32 m_timeBeginBake;
public:
	CViewBakeLightmap();

	virtual ~CViewBakeLightmap();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void copyColorBufferToMeshBuffer();
};