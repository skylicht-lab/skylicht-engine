#ifndef _VIEW_H_
#define _VIEW_H_

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

	virtual void onEvent(const SEvent& event);

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();
};

#endif