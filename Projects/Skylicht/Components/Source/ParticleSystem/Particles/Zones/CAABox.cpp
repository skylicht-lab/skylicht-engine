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
#include "CAABox.h"
#include "ParticleSystem/Particles/CParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		CAABox::CAABox(const core::vector3df& position, const core::vector3df& dimension) :
			CZone(AABox),
			m_position(position),
			m_dimension(dimension)
		{

		}

		CAABox::~CAABox()
		{

		}

		void CAABox::generatePosition(CParticle& particle, bool full)
		{
			core::vector3df pos = getTransformPosition(m_position);

			particle.Position.X = pos.X + random(-m_dimension.X * 0.5f, m_dimension.X * 0.5f);
			particle.Position.Y = pos.Y + random(-m_dimension.Y * 0.5f, m_dimension.Y * 0.5f);
			particle.Position.Z = pos.Z + random(-m_dimension.Z * 0.5f, m_dimension.Z * 0.5f);

			if (!full)
			{
				int axis = random(0, 3);
				int sens = (random(0, 2) << 1) - 1;

				switch (axis)
				{
				case 0:
					particle.Position.X = pos.X + sens * m_dimension.X * 0.5f;
					break;
				case 1:
					particle.Position.Y = pos.Y + sens * m_dimension.Y * 0.5f;
					break;
				default:
					particle.Position.Z = pos.Z + sens * m_dimension.Z * 0.5f;
					break;
				}
			}
		}

		core::vector3df CAABox::computeNormal(const core::vector3df& point)
		{
			core::vector3df pos = getTransformPosition(m_position);
			core::vector3df normal(point - pos);
			normal.normalize();
			return normal;
		}
	}
}