#include "pch.h"
#include "CTargetProjectile.h"

CTargetProjectile::CTargetProjectile(Particle::CGroup *group) :
	m_group(group),
	m_impactGroup(NULL)
{
	m_group->addCallback(this);
}

CTargetProjectile::~CTargetProjectile()
{
	m_group->removeCallback(this);
}

void CTargetProjectile::setTargetCollide(u32 index, const core::vector3df& pos, const core::vector3df& normal)
{
	if (index <= m_particleCollide.size())
	{
		SCollide &c = m_particleCollide[index];
		c.Position = pos;
		c.Normal = normal;
		c.HaveData = true;
	}
}

void CTargetProjectile::OnParticleUpdate(Particle::CParticle *particles, int num, Particle::CGroup *group, float dt)
{
	if (m_impactGroup == NULL)
		return;

	float minLength = 0.3f;
	float minLengthSQ = minLength * minLength;

	for (int i = 0; i < num; i++)
	{
		SCollide& c = m_particleCollide[i];

		if (c.HaveData == false)
			continue;

		Particle::CParticle &p = particles[i];
		if (p.Position.getDistanceFromSQ(c.Position) < minLengthSQ)
		{
			// add impact particle
			m_impactGroup->addParticle(0, c.Position, c.Normal);

			// kill this particle
			p.Life = -1.0f;
		}
	}
}

void CTargetProjectile::OnParticleBorn(Particle::CParticle &p)
{
	SCollide c;
	c.HaveData = false;

	m_particleCollide.push_back(c);
}

void CTargetProjectile::OnParticleDead(Particle::CParticle &p)
{
	m_particleCollide.set_used(m_particleCollide.size() - 1);
}

void CTargetProjectile::OnSwapParticleData(Particle::CParticle &p1, Particle::CParticle &p2)
{
	SCollide t = m_particleCollide[p1.Index];

	m_particleCollide[p1.Index] = m_particleCollide[p2.Index];
	m_particleCollide[p2.Index] = t;
}