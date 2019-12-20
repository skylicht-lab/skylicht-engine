#ifndef _VIEW_DEMO_H_
#define _VIEW_DEMO_H_

#include "ViewManager/CView.h"

class CViewDemo : public CView
{
public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();
};

#endif