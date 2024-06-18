#pragma once

#include "GameObject/CGameObject.h"
#include "Components/CComponentSystem.h"

using namespace Skylicht;

class TestComponent : public Skylicht::CComponentSystem
{
public:
	TestComponent();

	virtual ~TestComponent();

	virtual void initComponent();

	virtual void updateComponent();

};

void testComponent(CGameObject *gameObject);

bool isTestComponentPass();