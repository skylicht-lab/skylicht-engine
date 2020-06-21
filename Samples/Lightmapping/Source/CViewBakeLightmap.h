#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"
#include "Rasterisation/CRasterisation.h"

class CViewBakeLightmap : public CView
{
protected:
	std::vector<CRenderMesh*> m_renderMesh;

	std::vector<IMeshBuffer*> m_meshBuffers;
	core::array<core::matrix4> m_meshTransforms;

	CGameObject *m_bakeCameraObject;

	u32 m_currentPass;
	u32 m_currentMB;
	
	u32 m_currentTris;
	u32 m_lastTris;

	core::vector2di m_pixel;

	Lightmapper::CRasterisation *m_lmRasterize;
public:

	CViewBakeLightmap();

	virtual ~CViewBakeLightmap();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();
};