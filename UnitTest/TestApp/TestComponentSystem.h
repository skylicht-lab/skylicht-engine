#pragma once

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

bool isTestComponentPass();