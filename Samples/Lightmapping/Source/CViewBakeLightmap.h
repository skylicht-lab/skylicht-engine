#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"

class CViewBakeLightmap : public CView
{
protected:
	std::vector<CRenderMesh*> m_renderMesh;

	std::vector<IMeshBuffer*> m_meshBuffers;
	core::array<core::matrix4> m_meshTransforms;

	CGameObject *m_bakeCameraObject;
public:
	CViewBakeLightmap();

	virtual ~CViewBakeLightmap();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();
};