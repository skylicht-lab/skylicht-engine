#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"
#include "RenderMesh/CMesh.h"


class CViewDemo : public CView
{
protected:
	CGameObject* m_cat;

	struct SBlendShapeData
	{
		std::string Name;
		float Weight;
		std::vector<CBlendShape*> BlendShapes;
	};

	std::vector<SBlendShapeData*> m_blendshapes;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onGUI();

	SBlendShapeData* createGetBlendShapeByName(const char* name);

};