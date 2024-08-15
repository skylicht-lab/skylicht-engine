#pragma once

#include "Graphics2D/GUI/CGUIText.h"
#include "ViewManager/CView.h"

class CViewHeader : public CView
{
protected:
	CGUIText* m_txtUserName;

public:
	CViewHeader();

	virtual ~CViewHeader();

	virtual void onInit();

	virtual void onData();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onGUI();

};
