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
#include "CPoint.h"
#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CPoint::CPoint() :
			CPositionZone(Point)
		{

		}

		CPoint::~CPoint()
		{

		}

		CObjectSerializable* CPoint::createSerializable()
		{
			CObjectSerializable* object = CZone::createSerializable();
			object->autoRelease(new CVector3Property(object, "position", m_position));
			return object;
		}

		void CPoint::loadSerializable(CObjectSerializable* object)
		{
			CZone::loadSerializable(object);
			m_position = object->get<core::vector3df>("position", core::vector3df());
		}

		void CPoint::generatePosition(CParticle& particle, bool full, CGroup* group)
		{
			core::vector3df pos = group->getTransformPosition(m_position);
			particle.Position = pos;
		}

		core::vector3df CPoint::computeNormal(const core::vector3df& point, CGroup* group)
		{
			core::vector3df tpos = group->getTransformPosition(m_position);
			core::vector3df v = point - tpos;
			normalizeOrRandomize(v);
			return v;
		}
	}
}