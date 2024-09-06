#pragma once

#include "CParticle.h"
#include "CStick.h"

namespace Verlet
{
	class CVerlet
	{
	protected:
		core::array<CParticle*> m_particles;
		core::array<CStick*> m_sticks;

		core::vector3df m_gravity;
	public:
		CVerlet();

		virtual ~CVerlet();

		void update();

		void addParticle(int num);

		CStick* addStick(u32 p1, u32 p2);

		CStick* addStick(CParticle* p1, CParticle* p2);

		void clear();

		inline CParticle** getParticles()
		{
			return m_particles.pointer();
		}

		inline u32 getNumParticle()
		{
			return m_particles.size();
		}

		inline CStick** getSticks()
		{
			return m_sticks.pointer();
		}

		inline u32 getNumStick()
		{
			return m_sticks.size();
		}

		void drawDebug();

	protected:

		void simulation(float timestep);

		void updateParticle(float timestep);

		void updateStick(float timestep);

	};
}