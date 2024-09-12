#include "pch.h"
#include "CVerlet.h"
#include "Debug/CSceneDebug.h"

#include "Utils/CVector.h"

namespace Verlet
{
	CVerlet::CVerlet() :
		m_gravity(0.0f, -9.81f * 0.001f, 0.0f),
		m_firstUpdate(true)
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

	CParticle* CVerlet::addParticle()
	{
		m_particles.push_back(new CParticle());
		return m_particles.getLast();
	}

	CStickDistance* CVerlet::addStickDistance(u32 p1, u32 p2)
	{
		u32 n = m_particles.size();
		if (p1 >= n || p2 >= n)
			return NULL;

		return addStickDistance(m_particles[p1], m_particles[p2]);
	}

	CStickDistance* CVerlet::addStickDistance(CParticle* p1, CParticle* p2)
	{
		CStickDistance* stick = new CStickDistance(p1, p2);
		m_stickDistances.push_back(stick);
		return stick;
	}

	CStickAngle* CVerlet::addStickAngle(u32 p1, u32 p2, u32 p3)
	{
		u32 n = m_particles.size();
		if (p1 >= n || p2 >= n || p3 >= n)
			return NULL;

		return addStickAngle(m_particles[p1], m_particles[p2], m_particles[p3]);
	}

	CStickAngle* CVerlet::addStickAngle(CParticle* p1, CParticle* p2, CParticle* p3)
	{
		CStickAngle* stick = new CStickAngle(p1, p2, p3);
		m_stickAngles.push_back(stick);
		return stick;
	}

	void CVerlet::clear()
	{
		for (u32 i = 0, n = m_stickDistances.size(); i < n; i++)
			delete m_stickDistances[i];
		m_stickDistances.clear();

		for (u32 i = 0, n = m_stickAngles.size(); i < n; i++)
			delete m_stickAngles[i];
		m_stickAngles.clear();

		for (u32 i = 0, n = m_particles.size(); i < n; i++)
			delete m_particles[i];
		m_particles.clear();


		m_firstUpdate = true;
	}

	void CVerlet::update()
	{
		if (m_firstUpdate)
		{
			Verlet::CParticle** particles = m_particles.pointer();
			for (u32 i = 0, n = m_particles.size(); i < n; i++)
				particles[i]->OldPosition = particles[i]->Position;
			m_firstUpdate = false;
		}

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
		updateStickAngle(timestep);
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
			p->Velocity += p->AffectVelocity;

			p->OldPosition = p->Position;
			p->Position += m_gravity * timestep * p->Mass;
			p->Position += p->Velocity;

			p->AffectVelocity.set(0.0f, 0.0f, 0.0f);
		}
	}

	void CVerlet::updateStickDistance(float timestep)
	{
		// reference: https://github.com/subprotocol/verlet-js/blob/master/lib/constraint.js
		Verlet::CStickDistance** sticks = m_stickDistances.pointer();

		core::vector3df v;
		Verlet::CStickDistance* stick;

		float a, x;

		for (u32 i = 0, n = m_stickDistances.size(); i < n; i++)
		{
			stick = sticks[i];

			core::vector3df& p1Position = stick->P1->Position;
			core::vector3df& p2Position = stick->P2->Position;

			v = p2Position - p1Position;

			// see graph: y = b * (a - x)/x;
			a = stick->getDefaultDistance2();
			x = v.getLengthSQ();
			x = core::max_(x, 0.001f);

			stick->F = timestep * stick->Stiffness * ((a - x) / x);

			v = v * stick->F;

			if (!stick->P1->IsConstraint)
				p1Position -= v;

			if (!stick->P2->IsConstraint)
				p2Position += v;
		}
	}

	void CVerlet::updateStickAngle(float timestep)
	{
		Verlet::CStickAngle** sticks = m_stickAngles.pointer();
		Verlet::CStickAngle* stick;

		core::quaternion q;
		core::vector3df direction1, direction2;
		float coef;

		for (u32 i = 0, n = m_stickAngles.size(); i < n; i++)
		{
			stick = sticks[i];

			core::vector3df& p1 = stick->P1->Position;
			core::vector3df& p2 = stick->P2->Position;
			core::vector3df& p3 = stick->P3->Position;

			direction1 = p2 - p1;
			direction2 = p3 - p2;

			coef = timestep * stick->Stiffness;

			q.rotationFromTo(direction1, stick->Direction1);
			q *= coef;

			rotate(p1, p2, q);

			q.rotationFromTo(direction2, stick->Direction2);
			q *= coef;

			rotate(p2, p3, q);
		}
	}

	void CVerlet::rotate(const core::vector3df& origin, core::vector3df& pos, const core::quaternion& q)
	{
		core::vector3df v = pos - origin;
		float l = v.getLength();
		v.normalize();
		v = q * v;
		pos = origin + v * l;
	}

	void CVerlet::drawDebug(bool drawingParticle)
	{
		CSceneDebug* debug = CSceneDebug::getInstance();

		SColor colorWhite(255, 200, 200, 200);
		SColor colorRed(255, 200, 0, 0);
		SColor colorGreen(255, 0, 200, 0);

		CStickDistance** stickDistances = m_stickDistances.pointer();

		for (u32 i = 0, n = m_stickDistances.size(); i < n; i++)
		{
			CStickDistance* s = stickDistances[i];

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

		for (u32 i = 0, n = m_stickAngles.size(); i < n; i++)
		{
			CStickAngle* s = m_stickAngles[i];
			debug->addLine(
				s->P1->Position,
				s->P1->Position + s->Direction1 * 0.2f,
				colorGreen
			);

			debug->addLine(
				s->P2->Position,
				s->P2->Position + s->Direction2 * 0.2f,
				colorGreen
			);
		}

		if (drawingParticle)
		{
			CParticle** particles = m_particles.pointer();
			for (u32 i = 0, n = m_particles.size(); i < n; i++)
			{
				CParticle* particle = particles[i];

				debug->addSphere(
					particle->Position,
					0.02f,
					particle->IsConstraint ? colorRed : colorWhite
				);
			}
		}
	}
}