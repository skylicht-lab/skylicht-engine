#pragma once

#include "ViewManager/CView.h"

class CViewBakeLightmap : public CView
{
protected:
	CGameObject *m_guiObject;
	CGameObject *m_bakeCameraObject;

	CGUIText *m_textInfo;
	CGlyphFont *m_font;

	std::vector<CRenderMesh*> m_renderMesh;
	std::vector<IMeshBuffer*> m_allMeshBuffer;

	core::array<SColor> m_color;

	u32 m_currentMeshBuffer;
	u32 m_currentVertex;

public:
	CViewBakeLightmap();

	virtual ~CViewBakeLightmap();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();
};