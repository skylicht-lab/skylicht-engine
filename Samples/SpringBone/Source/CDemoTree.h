#pragma once

#include "CDemo.h"

class CDemoTree : public CDemo
{
protected:

public:
	CDemoTree(Verlet::CVerlet* verlet);

	virtual ~CDemoTree();

	virtual void init();

	virtual void update();

	virtual void onGUI();
};