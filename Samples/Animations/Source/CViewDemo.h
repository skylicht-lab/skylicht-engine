#pragma once

#include "ViewManager/CView.h"
#include "Animation/CGradientBandInterpolation.h"
#include "Animation/CAnimationController.h"

class CViewDemo : public CView
{
protected:
	CGradientBandInterpolation m_movementBlending;
	CAnimationController* m_animController;

	float m_movementSpeed;
	float m_movementAngle;
	bool m_modify;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void onGUI();
};