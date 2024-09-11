#pragma once

#include "CParticle.h"
#include "CStick.h"

namespace Verlet
{
	class CVerlet
	{
	protected:
		core::array<CParticle*> m_particles;
		core::array<CStickDistance*> m_stickDistances;

		core::vector3df m_gravity;
	public:
		CVerlet();

		virtual ~CVerlet();

		void update();

		void addParticle(int num);

		CStickDistance* addStickDistance(u32 p1, u32 p2);

		CStickDistance* addStickDistance(CParticle* p1, CParticle* p2);

		void clear();

		inline CParticle** getParticles()
		{
			return m_particles.pointer();
		}

		inline u32 getNumParticle()
		{
			return m_particles.size();
		}

		inline CStickDistance** getStickDistances()
		{
			return m_stickDistances.pointer();
		}

		inline u32 getNumStickDistance()
		{
			return m_stickDistances.size();
		}

		void drawDebug();

		inline void setGravity(const core::vector3df& g)
		{
			m_gravity = g;
		}

		inline const core::vector3df& getGravity()
		{
			return m_gravity;
		}

	protected:

		void simulation(float timestep);

		void updateParticle(float timestep);

		void updateStickDistance(float timestep);

	};
}