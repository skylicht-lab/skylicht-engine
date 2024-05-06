#include "pch.h"
#include "CBoldData.h"

IMPLEMENT_DATA_TYPE_INDEX(CBoldData);

CBoldData::CBoldData() :
	MaxSpeed(0.0f),
	MaxForce(0.0f),
	Alive(true)
{
	MaxSpeed = 2.5f;
	MaxForce = 0.25f;

	float rx = os::Randomizer::frand() * 2.0f - 1.0f;
	float rz = os::Randomizer::frand() * 2.0f - 1.0f;

	Velocity.set(rx, 0.0f, rz);

	Front = Velocity;
	Front.normalize();
}

CBoldData::~CBoldData()
{

}