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

#pragma once

#include "CZone.h"

namespace Skylicht
{
	namespace Particle
	{
		class CCylinder : public CZone
		{
		protected:
			core::vector3df m_position;
			core::vector3df m_direction;
			float m_radius;
			float m_length;

		public:
			CCylinder(const core::vector3df& position,
				const core::vector3df& direction,
				float radius,
				float length);

			virtual ~CCylinder();

			inline void setPosition(const core::vector3df& pos)
			{
				m_position = pos;
			}

			inline void setDirection(const core::vector3df& direction)
			{
				m_direction = direction;
				m_direction.normalize();
			}

			inline void setRadius(float r)
			{
				m_radius = r;
			}

			inline void setLength(float l)
			{
				m_length = l;
			}

			inline const core::vector3df& getPosition()
			{
				return m_position;
			}

			inline const core::vector3df& getDirection()
			{
				return m_direction;
			}

			inline float getRadius()
			{
				return m_radius;
			}

			inline float getLength()
			{
				return m_length;
			}

			virtual void generatePosition(CParticle& particle, bool full);

			virtual core::vector3df computeNormal(const core::vector3df& point);
		};
	}
}