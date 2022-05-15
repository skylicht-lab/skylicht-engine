#pragma once

class CView
{
protected:

public:
	CView();

	virtual ~CView();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onActive();

	virtual void onDeactive();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();
};