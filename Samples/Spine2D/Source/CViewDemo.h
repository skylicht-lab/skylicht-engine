#pragma once

#include "ViewManager/CView.h"
#include "Graphics2D/GUI/CGUIElement.h"
#include "CSpineResource.h"

class CViewDemo : public CView
{
protected:
	spine::CSpineResource* m_spineResource;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void renderSpine(CGUIElement* element);

	void onGUI();

};