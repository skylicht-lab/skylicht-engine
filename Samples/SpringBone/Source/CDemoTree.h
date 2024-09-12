#pragma once

#include "CDemo.h"

class CDemoTree : public CDemo
{
protected:
	
	float m_branchLength;
	float m_branchAngle;
	core::vector3df m_branchRotateAxis;

public:
	CDemoTree(Verlet::CVerlet* verlet);

	virtual ~CDemoTree();

	virtual void init();

	virtual void update();

	virtual void onGUI();

protected:

	Verlet::CParticle* branch(Verlet::CParticle* parent, int depth, int maxDepth, float lengthScale, const core::vector3df& normal);

};