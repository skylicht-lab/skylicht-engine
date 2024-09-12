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
		core::array<CStickAngle*> m_stickAngles;

		core::vector3df m_gravity;

		bool m_firstUpdate;
	public:
		CVerlet();

		virtual ~CVerlet();

		void update();

		void addParticle(int num);

		CParticle* addParticle();

		CStickDistance* addStickDistance(u32 p1, u32 p2);

		CStickDistance* addStickDistance(CParticle* p1, CParticle* p2);

		CStickAngle* addStickAngle(u32 p1, u32 p2, u32 p3);

		CStickAngle* addStickAngle(CParticle* p1, CParticle* p2, CParticle* p3);

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

		void drawDebug(bool drawingParticle);

		inline void setGravity(const core::vector3df& g)
		{
			m_gravity = g;
		}

		inline const core::vector3df& getGravity()
		{
			return m_gravity;
		}

		inline void setDefaultGravity()
		{
			m_gravity.set(0.0f, -9.81f * 0.001f, 0.0f);
		}

	protected:

		void simulation(float timestep);

		void updateParticle(float timestep);

		void updateStickDistance(float timestep);

		void updateStickAngle(float timestep);

		void rotate(const core::vector3df& origin, core::vector3df& pos, const core::quaternion& q);

	};
}