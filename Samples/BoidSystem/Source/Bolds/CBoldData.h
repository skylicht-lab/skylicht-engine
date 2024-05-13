#pragma once

#include "Entity/IEntityData.h"
#include "Entity/CArrayUtils.h"

class CBoldData : public Skylicht::IEntityData
{
public:
	core::vector3df Location;
	core::vector3df Velocity;
	core::vector3df Acceleration;
	core::vector3df Front;

	float MaxSpeed;
	float MaxForce;
	bool Alive;

	CFastArray<CBoldData*> Neighbor;

	CBoldData();
	~CBoldData();
};

DECLARE_PRIVATE_DATA_TYPE_INDEX(CBoldData);