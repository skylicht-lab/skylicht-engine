#include "pch.h"
#include "CDemoCloth.h"
#include "imgui.h"

CDemoCloth::CDemoCloth(Verlet::CVerlet* verlet) :
	CDemo(verlet),
	m_moveParticle(false),
	m_moveId(0),
	m_moveValue(0.0f),
	m_moveRadius(0.0f)
{

}

CDemoCloth::~CDemoCloth()
{

}

void CDemoCloth::init()
{
	int numCol = 32;
	int numRow = 32;

	int numParticles = numCol * numRow;

	m_moveValue = 0.0f;

	m_verlet->clear();
	m_verlet->addParticle(numParticles);

	Verlet::CParticle** particles = m_verlet->getParticles();

	float space = 0.1f;
	core::vector3df offset(0.0f, numRow * space, 0.0f);

	for (int y = 0; y < numRow; y++)
	{
		for (int x = 0; x < numCol; x++)
		{
			int id = y * numCol + x;

			Verlet::CParticle* p = particles[id];
			p->setPosition(offset + core::vector3df(x * space, -y * space, 0.0f));
		}
	}

	m_moveId = numCol - 1;

	particles[0]->IsConstraint = true;
	particles[m_moveId]->IsConstraint = true;

	m_moveRadius = particles[m_moveId]->Position.X;

	for (int y = 0; y < numRow; y++)
	{
		for (int x = 0; x < numCol; x++)
		{
			if (x < numCol - 1)
			{
				m_verlet->addStickDistance(
					particles[y * numCol + x],
					particles[y * numCol + x + 1]
				);
			}

			if (y < numRow - 1)
			{
				m_verlet->addStickDistance(
					particles[y * numCol + x],
					particles[(y + 1) * numCol + x]
				);
			}
		}
	}
}

void CDemoCloth::update()
{
	if (m_moveParticle)
	{
		Verlet::CParticle** particles = m_verlet->getParticles();

		m_moveValue = m_moveValue + 0.001f * getTimeStep();
		m_moveValue = fmodf(m_moveValue, 2.0f * core::PI);

		core::vector3df& pos = particles[m_moveId]->Position;
		pos.X = cosf(m_moveValue) * m_moveRadius;
		pos.Z = sinf(m_moveValue) * m_moveRadius;
	}
}

void CDemoCloth::onGUI()
{
	ImGui::Checkbox("Move cloth particle", &m_moveParticle);
}