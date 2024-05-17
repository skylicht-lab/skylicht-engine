#pragma once

#include "ViewManager/CView.h"
#include "VertexAnimation/CRenderMeshInstancingVAT.h"

#include "Tween/CTweenManager.h"

class CViewDemo : public CView
{
protected:
	std::vector<CRenderMeshInstancingVAT*> m_instancings;

	CEditorCamera* m_editorCam;
	C3rdCamera* m_followRotateCam;
	C3rdCamera* m_followTopCam;
	CCameraBrain* m_cameraBrain;

	CTweenFloat* m_cameraBlendTween;
	CEntity* m_followEntity;

	bool m_debugNeighbor;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void onGUI();

	void setTargetCamera(CCamera* cam, float blendDuration);

	void followRandomEntity();

	void testSpawnEntity(int count);
};