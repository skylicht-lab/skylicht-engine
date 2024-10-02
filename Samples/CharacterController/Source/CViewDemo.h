#pragma once

#include "ViewManager/CView.h"
#include "RigidBody/CRigidbody.h"
#include "CharacterController/CCharacterController.h"

class CViewDemo : public CView
{
protected:
	Physics::CCharacterController* m_player;
	
	std::vector<Physics::CRigidbody*> m_boxs;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void initObjects();

	void onGUI();
};