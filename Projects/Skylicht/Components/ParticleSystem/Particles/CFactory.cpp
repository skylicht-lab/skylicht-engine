/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CFactory.h"

namespace Skylicht
{
	namespace Particle
	{
		CFactory::CFactory()
		{

		}

		CFactory::~CFactory()
		{
			for (IRenderer* r : m_renderers)
				delete r;

			for (CZone* z : m_zones)
				delete z;

			for (CEmitter* e : m_emitters)
				delete e;

			m_renderers.clear();
			m_zones.clear();
			m_emitters.clear();
		}

		CRandomEmitter* CFactory::createRandomEmitter()
		{
			CRandomEmitter* e = new CRandomEmitter();
			m_emitters.push_back(e);
			return e;
		}

		CStraightEmitter* CFactory::createStraightEmitter(const core::vector3df& direction)
		{
			CStraightEmitter* e = new CStraightEmitter();
			e->setDirection(direction, true);
			m_emitters.push_back(e);
			return e;
		}

		CSphericEmitter* CFactory::createSphericEmitter(const core::vector3df& direction, float angleA, float angleB)
		{
			CSphericEmitter* e = new CSphericEmitter();
			e->setDirection(direction, true);
			e->setAngles(angleA, angleB);
			m_emitters.push_back(e);
			return e;
		}

		CNormalEmitter* CFactory::createNormalEmitter(bool inverted)
		{
			CNormalEmitter* e = new CNormalEmitter();
			e->setInverted(inverted);
			m_emitters.push_back(e);
			return e;
		}

		void CFactory::deleteEmitter(CEmitter* e)
		{
			std::vector<CEmitter*>::iterator i = std::find(m_emitters.begin(), m_emitters.end(), e);
			if (i != m_emitters.end())
			{
				m_emitters.erase(i);
				delete e;
			}
		}

		CQuadRenderer* CFactory::createQuadRenderer()
		{
			CQuadRenderer* r = new CQuadRenderer();
			m_renderers.push_back(r);
			return r;
		}

		CCPURenderer* CFactory::createCPURenderer()
		{
			CCPURenderer* r = new CCPURenderer();
			m_renderers.push_back(r);
			return r;
		}

		CMeshParticleRenderer* CFactory::createMeshParticleRenderer()
		{
			CMeshParticleRenderer* r = new CMeshParticleRenderer();
			m_renderers.push_back(r);
			return r;
		}

		void CFactory::deleteRenderer(IRenderer* r)
		{
			std::vector<IRenderer*>::iterator i = std::find(m_renderers.begin(), m_renderers.end(), r);
			if (i != m_renderers.end())
			{
				m_renderers.erase(i);
				delete r;
			}
		}

		CZone* CFactory::createZone(EZone type)
		{
			CZone* zone = NULL;
			switch (type)
			{
			case Particle::EZone::Point:
				zone = createPointZone();
				break;
			case Particle::EZone::Sphere:
				zone = createSphereZone(core::vector3df(), 1.0f);
				break;
			case Particle::EZone::AABox:
				zone = createAABoxZone(core::vector3df(), core::vector3df(1.0f, 1.0f, 1.0f));
				break;
			case Particle::EZone::Cylinder:
				zone = createCylinderZone(core::vector3df(), core::vector3df(0.0f, 1.0f, 0.0f), 1.0f, 1.0f);
				break;
			case Particle::EZone::Line:
				zone = createLineZone(core::vector3df(0.0f, 0.0f, -1.0f), core::vector3df(0.0f, 0.0f, 1.0f));
				break;
			case Particle::EZone::PolyLine:
			{
				core::array<core::vector3df> points;
				float size = 1.0f;
				points.push_back(core::vector3df(-size, 0.0f, -size));
				points.push_back(core::vector3df(size, 0.0f, -size));
				points.push_back(core::vector3df(size, 0.0f, size));
				points.push_back(core::vector3df(-size, 0.0f, size));
				points.push_back(core::vector3df(-size, 0.0f, -size));
				zone = createPolyLineZone(points);
			}
			break;
			case Particle::EZone::Ring:
				zone = createRingZone(core::vector3df(), core::vector3df(0.0f, 1.0f, 0.0f), 1.0f, 1.5f);
				break;
			default:
				break;
			}
			return zone;
		}

		CPoint* CFactory::createPointZone()
		{
			CPoint* z = new CPoint();
			m_zones.push_back(z);
			return z;
		}

		CPoint* CFactory::createPointZone(const core::vector3df& pos)
		{
			CPoint* z = new CPoint();
			z->setPosition(pos);
			m_zones.push_back(z);
			return z;
		}

		CSphere* CFactory::createSphereZone(const core::vector3df& pos, float radius)
		{
			CSphere* z = new CSphere(pos, radius);
			m_zones.push_back(z);
			return z;
		}

		CAABox* CFactory::createAABoxZone(const core::vector3df& pos, const core::vector3df& dimension)
		{
			CAABox* z = new CAABox(pos, dimension);
			m_zones.push_back(z);
			return z;
		}

		CCylinder* CFactory::createCylinderZone(const core::vector3df& pos, const core::vector3df& direction, float radius, float length)
		{
			CCylinder* z = new CCylinder(pos, direction, radius, length);
			m_zones.push_back(z);
			return z;
		}

		CLine* CFactory::createLineZone(const core::vector3df& p1, const core::vector3df& p2)
		{
			CLine* z = new CLine(p1, p2);
			m_zones.push_back(z);
			return z;
		}

		CPolyLine* CFactory::createPolyLineZone(const core::array<core::vector3df>& points)
		{
			if (points.size() < 2)
				return NULL;

			CPolyLine* z = new CPolyLine(points);
			m_zones.push_back(z);
			return z;
		}

		CRing* CFactory::createRingZone(const core::vector3df& pos, const core::vector3df& normal, float minRadius, float maxRadius)
		{
			CRing* z = new CRing(pos, normal, minRadius, maxRadius);
			m_zones.push_back(z);
			return z;
		}

		void CFactory::deleteZone(CZone* z)
		{
			std::vector<CZone*>::iterator i = std::find(m_zones.begin(), m_zones.end(), z);
			if (i != m_zones.end())
			{
				m_zones.erase(i);
				delete z;
			}
		}

		CEmitter* CFactory::createEmitter(const std::wstring& attributeName)
		{
			CEmitter* emitter = NULL;

			if (attributeName == L"CNormalEmitter")
				emitter = createNormalEmitter(false);
			else if (attributeName == L"CRandomEmitter")
				emitter = createRandomEmitter();
			else if (attributeName == L"CSphericEmitter")
				emitter = createSphericEmitter(Transform::Oy, 0.0f, 45.0f * core::DEGTORAD);
			else if (attributeName == L"CStraightEmitter")
				emitter = createStraightEmitter(Transform::Oy);

			return emitter;
		}

		CZone* CFactory::createZone(const std::wstring& attributeName)
		{
			CZone* zone = NULL;

			if (attributeName == L"CAABox")
				zone = createZone(AABox);
			else if (attributeName == L"CCylinder")
				zone = createZone(Cylinder);
			else if (attributeName == L"CLine")
				zone = createZone(Line);
			else if (attributeName == L"CPoint")
				zone = createZone(Point);
			else if (attributeName == L"CPolyLine")
				zone = createZone(PolyLine);
			else if (attributeName == L"CRing")
				zone = createZone(Ring);
			else if (attributeName == L"CSphere")
				zone = createZone(Sphere);

			return zone;
		}

		IRenderer* CFactory::createRenderer(const std::wstring& attributeName)
		{
			IRenderer* renderer = NULL;
			if (attributeName == L"CQuadRenderer")
				renderer = createQuadRenderer();
			else if (attributeName == L"CCPURenderer")
				renderer = createCPURenderer();
			else if (attributeName == L"CMeshParticleRenderer")
				renderer = createMeshParticleRenderer();
			return renderer;
		}
	}
}