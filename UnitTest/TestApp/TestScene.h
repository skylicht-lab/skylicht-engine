#pragma once

#include "Base.hh"
#include "Scene/CScene.h"

#include "TestComponentSystem.h"

class TestScene
{
protected:
	CScene				*m_scene;
	CZone				*m_zone;

	int					m_testStep;

public:
	TestScene();

	virtual ~TestScene();

	virtual void update();

};

void testScene();

void testSceneUpdate();

bool isTestScenePass();