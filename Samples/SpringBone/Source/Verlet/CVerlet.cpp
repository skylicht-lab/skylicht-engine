#include "pch.h"
#include "CVerlet.h"
#include "Debug/CSceneDebug.h"

namespace Verlet
{
	CVerlet::CVerlet() :
		m_gravity(0.0f, -9.81f * 0.001f, 0.0f)
	{

	}

	CVerlet::~CVerlet()
	{
		clear();
	}

	void CVerlet::addParticle(int num)
	{
		for (int i = 0; i < num; i++)
		{
			m_particles.push_back(new CParticle());
		}
	}

	CStick* CVerlet::addStick(u32 p1, u32 p2)
	{
		u32 n = m_particles.size();
		if (p1 >= n || p2 >= n)
			return NULL;

		return addStick(m_particles[p1], m_particles[p2]);
	}

	CStick* CVerlet::addStick(CParticle* p1, CParticle* p2)
	{
		CStick* stick = new CStick(p1, p2);
		m_sticks.push_back(stick);
		return stick;
	}

	void CVerlet::clear()
	{
		for (u32 i = 0, n = m_sticks.size(); i < n; i++)
			delete m_sticks[i];
		m_sticks.clear();

		for (u32 i = 0, n = m_particles.size(); i < n; i++)
			delete m_particles[i];
		m_particles.clear();
	}

	void CVerlet::update()
	{
		float ts = getTimeStep();

		float maxTs = 1000.0f / 30.0f;
		if (ts > maxTs)
			ts = maxTs;

		int numIterations = 5;
		float stepTime = ts / (float)numIterations;

		for (int i = 0; i < numIterations; i++)
			simulation(stepTime);
	}

	void CVerlet::simulation(float timestep)
	{
		updateParticle(timestep);
		updateStickDistance(timestep);
	}

	void CVerlet::updateParticle(float timestep)
	{
		Verlet::CParticle** particles = m_particles.pointer();

		for (u32 i = 0, n = m_particles.size(); i < n; i++)
		{
			Verlet::CParticle* p = particles[i];

			if (p->IsConstraint)
				continue;

			p->Velocity = (p->Position - p->OldPosition) * p->Friction;

			p->OldPosition = p->Position;
			p->Position += m_gravity * timestep * p->Mass;
			p->Position += p->Velocity;
		}
	}

	void CVerlet::updateStickDistance(float timestep)
	{
		// reference: https://github.com/subprotocol/verlet-js/blob/master/lib/constraint.js
		Verlet::CStick** sticks = m_sticks.pointer();

		core::vector3df v;
		Verlet::CStick* stick;

		float a, x;

		for (u32 i = 0, n = m_sticks.size(); i < n; i++)
		{
			stick = sticks[i];

			core::vector3df& p1Position = stick->P1->Position;
			core::vector3df& p2Position = stick->P2->Position;

			v = p2Position - p1Position;

			// see graph: y = b * (a - x)/x;
			a = stick->getDefaultDistance2();
			x = v.getLengthSQ();
			x = core::max_(x, a);

			stick->F = timestep * stick->Stiffness * ((a - x) / x);

			v = v * stick->F;

			if (!stick->P1->IsConstraint)
				p1Position -= v;

			if (!stick->P2->IsConstraint)
				p2Position += v;
		}
	}

	void CVerlet::drawDebug()
	{
		CSceneDebug* debug = CSceneDebug::getInstance();

		SColor colorWhite(255, 200, 200, 200);
		SColor colorRed(255, 200, 0, 0);

		CStick** sticks = m_sticks.pointer();

		for (u32 i = 0, n = m_sticks.size(); i < n; i++)
		{
			CStick* s = sticks[i];

			if (s->DebugColor)
			{
				debug->addLine(
					s->P1->Position,
					s->P2->Position,
					s->Color
				);
			}
			else
			{
				bool isConstraint = s->P1->IsConstraint || s->P2->IsConstraint;
				debug->addLine(
					s->P1->Position,
					s->P2->Position,
					isConstraint ? colorRed : colorWhite
				);
			}
		}
	}
}