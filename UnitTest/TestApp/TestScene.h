#pragma once

#include "Base.hh"
#include "Scene/CScene.h"
#include "TestComponentSystem.h"
#include "Camera/CCamera.h"

class TestScene
{
protected:
	CScene			*m_scene;
	CZone			*m_zone;
	CCamera			*m_camera;

	CForwardRP		*m_forwardRP;
	CDeferredRP		*m_deferredRP;

	int				m_testStep;

public:
	TestScene();

	virtual ~TestScene();

	virtual void update();

	virtual void render();

};

void testScene();

void testSceneUpdate();

void testSceneRender();

bool isTestScenePass();