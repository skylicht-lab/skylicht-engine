#pragma once

#include "GameObject/CZone.h"

class CDemo
{
protected:
	CZone* m_zone;

public:
	CDemo(CZone* zone);

	virtual ~CDemo();

	virtual void init() = 0;

	virtual void update() = 0;

	virtual void onGUI() = 0;

	virtual void onLeftClickPosition(bool holdShift, const core::vector3df& pos);

};