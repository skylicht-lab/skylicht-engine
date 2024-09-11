#pragma once

#include "Verlet/CVerlet.h"

class CDemo
{
protected:
	Verlet::CVerlet* m_verlet;

public:
	CDemo(Verlet::CVerlet* verlet);

	virtual ~CDemo();

	virtual void init() = 0;

	virtual void update() = 0;

	virtual void onGUI() = 0;
};