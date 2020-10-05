#pragma once

#include "ParticleSystem/Particles/CGroup.h"
#include "ParticleSystem/Particles/Systems/ISystem.h"

class CTargetProjectile :public Particle::IParticleCallback
{
protected:
	struct SCollide
	{
		core::vector3df Position;
		core::vector3df Normal;
		bool HaveData;
	};

	core::array<SCollide> m_particleCollide;

	Particle::CGroup *m_group;
	Particle::CGroup *m_impactGroup;

public:
	CTargetProjectile(Particle::CGroup *group);

	virtual ~CTargetProjectile();

	void setTargetCollide(u32 index, const core::vector3df& pos, const core::vector3df& normal);

	inline void setImpactGroup(Particle::CGroup *g)
	{
		m_impactGroup = g;
	}

	virtual void OnParticleUpdate(Particle::CParticle *particles, int num, Particle::CGroup *group, float dt);

	virtual void OnParticleBorn(Particle::CParticle &p);

	virtual void OnParticleDead(Particle::CParticle &p);

	virtual void OnSwapParticleData(Particle::CParticle &p1, Particle::CParticle &p2);
};