#include "pch.h"
#include "CDemoTree.h"

CDemoTree::CDemoTree(Verlet::CVerlet* verlet) :
	CDemo(verlet),
	m_branchLength(2.0f),
	m_branchAngle(38.0f * core::DEGTORAD),
	m_branchRotateAxis(0.0f, 0.0f, 1.0f)
{
	m_drawingParticle = true;
	m_enableWind = false;
}

CDemoTree::~CDemoTree()
{

}

void CDemoTree::init()
{
	m_verlet->clear();
	m_verlet->setGravity(core::vector3df());

	Verlet::CParticle* root = m_verlet->addParticle();
	root->IsConstraint = true;

	Verlet::CParticle* base = m_verlet->addParticle();
	base->IsConstraint = true;
	base->Position.set(0.0f, 0.2f, 0.0f);

	m_verlet->addStickDistance(root, base);

	float lengthScale = 0.7f;

	m_branchRotateAxis.set(0.0f, 0.0f, 1.0f);
	Verlet::CParticle* mainBranch;

	mainBranch = branch(base, 0, 4, lengthScale, core::vector3df(0.0f, 1.0f, 0.0f));
	m_verlet->addStickDistance(base, mainBranch);
	m_verlet->addStickAngle(root, base, mainBranch);

	m_branchRotateAxis.set(1.0f, 0.0f, 0.0f);
	mainBranch = branch(base, 0, 4, lengthScale, core::vector3df(0.0f, 1.0f, 0.0f));
	m_verlet->addStickDistance(base, mainBranch);
	m_verlet->addStickAngle(root, base, mainBranch);
}

Verlet::CParticle* CDemoTree::branch(Verlet::CParticle* parent, int depth, int maxDepth, float lengthScale, const core::vector3df& normal)
{
	// Reference:
	// https://github.com/subprotocol/verlet-js/blob/master/examples/tree.html

	Verlet::CParticle* particle = m_verlet->addParticle();
	particle->Position = parent->Position + normal * m_branchLength * lengthScale;
	particle->Friction = 0.985f;

	m_verlet->addStickDistance(parent, particle);

	if (depth < maxDepth)
	{
		lengthScale = lengthScale * 0.6f;

		core::quaternion q;
		core::vector3df branchNormal;

		q.fromAngleAxis(-m_branchAngle, m_branchRotateAxis);
		branchNormal = q * normal;
		branchNormal.normalize();

		Verlet::CParticle* a = branch(particle, depth + 1, maxDepth, lengthScale, branchNormal);

		q.fromAngleAxis(m_branchAngle, m_branchRotateAxis);
		branchNormal = q * normal;
		branchNormal.normalize();

		Verlet::CParticle* b = branch(particle, depth + 1, maxDepth, lengthScale, branchNormal);

		m_verlet->addStickAngle(parent, particle, a);
		m_verlet->addStickAngle(parent, particle, b);
	}

	return particle;
}

void CDemoTree::update()
{

}

void CDemoTree::onGUI()
{

}