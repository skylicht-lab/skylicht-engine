#include "pch.h"
#include "CMoveAgent.h"

#include "GameObject/CGameObject.h"
#include "Debug/CSceneDebug.h"
#include "Utils/CVector.h"

CMoveAgent::CMoveAgent() :
	m_obstacle(NULL),
	m_graphQuery(NULL),
	m_agentRadius(0.5f)
{
	m_obstacle = new Graph::CObstacleAvoidance();
}

CMoveAgent::~CMoveAgent()
{
	delete m_obstacle;
}

void CMoveAgent::initComponent()
{

}

void CMoveAgent::updateComponent()
{
	core::vector3df heightOffset(0.0f, 1.0f, 0.0f);
	CTransformEuler* transform = m_gameObject->getTransformEuler();

	core::vector3df position = transform->getPosition() - heightOffset;
	core::vector3df newPosition;

	if (m_points.size() > 0)
	{
		// go by path
		newPosition = folowPath();
	}
	else
	{
		// no path, go straight
		newPosition = CVector::lerp(position, m_targetPosition, 0.001f * getTimeStep());
	}

	if (m_obstacle)
	{
		if (m_graphQuery)
		{
			core::aabbox3df box;
			core::vector3df r(m_agentRadius, 0.0f, m_agentRadius);

			box.MinEdge = newPosition - r;
			box.MaxEdge = newPosition + core::vector3df(0.0f, 2.0f, 0.0f) + r;

			m_obstacle->clear();
			m_graphQuery->getObstacles(box, *m_obstacle);

			/*
			core::array<core::line3df>& segs = m_obstacle->getSegments();
			SColor c(255, 255, 0, 255);
			for (u32 i = 0, n = segs.size(); i < n; i++)
			{
				CSceneDebug::getInstance()->addLine(segs[i], c);
			}
			*/
		}

		core::vector3df velocity = newPosition - position;
		newPosition = m_obstacle->collide(position, velocity, m_agentRadius);
	}

	transform->setPosition(newPosition + heightOffset);
}

void CMoveAgent::setPosition(const core::vector3df& position)
{
	core::vector3df heightOffset(0.0f, 1.0f, 0.0f);
	CTransformEuler* transform = m_gameObject->getTransformEuler();
	transform->setPosition(position + heightOffset);
}

core::vector3df CMoveAgent::getPosition()
{
	core::vector3df heightOffset(0.0f, 1.0f, 0.0f);
	CTransformEuler* transform = m_gameObject->getTransformEuler();
	return transform->getPosition() - heightOffset;
}

void CMoveAgent::setPath(const core::array<Graph::STile*>& path, const core::vector3df& target)
{
	m_points.clear();
	m_distance = 0.0f;
	m_moveTime = 0.0f;

	if (path.size() == 0)
		return;

	m_points.push_back(getPosition());
	for (u32 i = 1, n = path.size() - 1; i < n; i++)
		m_points.push_back(path[i]->Position);
	m_points.push_back(target);

	for (u32 i = 0, n = m_points.size() - 1; i < n; i++)
	{
		m_distance = m_distance + m_points[i].getDistanceFrom(m_points[i + 1]);
	}
}

core::vector3df CMoveAgent::folowPath()
{
	// see Irrlicht source: CSceneNodeAnimatorFollowSpline::animateNode
	const float Speed = 4.0f;
	const float Tightness = 0.1f;

	m_moveTime = m_moveTime + getTimeStep();

	u32 idx = 0;
	f32 u = 0.0f;

	f32 distance = (m_moveTime * Speed * 0.001f);
	if (distance >= m_distance)
	{
		// finish
		idx = m_points.size() - 1;
	}
	else
	{
		float d1 = 0.0f, d2 = 0.0f;
		for (u32 i = 0, n = m_points.size() - 1; i < n; i++)
		{
			d1 = d1 + m_points[i].getDistanceFrom(m_points[i + 1]);
			if (d1 > distance)
			{
				idx = i;
				u = (distance - d2) / (d1 - d2);
				break;
			}
			d2 = d1;
		}
	}

	u32 nPoint = m_points.size() - 1;
	const core::vector3df& p0 = m_points[core::clamp<u32>(idx - 1, 0, nPoint)];
	const core::vector3df& p1 = m_points[core::clamp<u32>(idx + 0, 0, nPoint)]; // starting point
	const core::vector3df& p2 = m_points[core::clamp<u32>(idx + 1, 0, nPoint)]; // end point
	const core::vector3df& p3 = m_points[core::clamp<u32>(idx + 2, 0, nPoint)];

	// hermite polynomials
	const f32 h1 = 2.0f * u * u * u - 3.0f * u * u + 1.0f;
	const f32 h2 = -2.0f * u * u * u + 3.0f * u * u;
	const f32 h3 = u * u * u - 2.0f * u * u + u;
	const f32 h4 = u * u * u - u * u;

	// tangents
	const core::vector3df t1 = (p2 - p0) * Tightness;
	const core::vector3df t2 = (p3 - p1) * Tightness;

	// interpolated point
	core::vector3df result = (p1 * h1 + p2 * h2 + t1 * h3 + t2 * h4);
	return result;
}