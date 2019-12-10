#pragma once

#include "GameObject/CGameObject.h"
#include "Components/CComponentSystem.h"

class TestComponent : public Skylicht::CComponentSystem
{
public:
	TestComponent();

	virtual ~TestComponent();

	virtual void initComponent();

	virtual void updateComponent();

	virtual void postUpdateComponent();

	virtual void endUpdate();

};

void testComponent(CGameObject *gameObject);

bool isTestComponentPass();