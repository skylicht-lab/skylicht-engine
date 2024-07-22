#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"
#include "RenderMesh/CMesh.h"
#include "Animation/CAnimationController.h"

class CViewHeader : public CView
{
protected:

public:
	CViewHeader();

	virtual ~CViewHeader();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onGUI();

};
