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

#include "CPositionZone.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CCylinder
		 * @ingroup ParticleSystem
		 * @brief A zone that spawns particles within or on the surface of a cylinder.
		 */
		class COMPONENT_API CCylinder : public CPositionZone
		{
		protected:
			/** @brief Central axis direction. */
			core::vector3df m_direction;
			/** @brief Radius of the cylinder. */
			float m_radius;
			/** @brief Length (height) along the axis. */
			float m_length;

		public:
			CCylinder(const core::vector3df& position,
				const core::vector3df& direction,
				float radius,
				float length);

			virtual ~CCylinder();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Sets central axis direction. */
			inline void setDirection(const core::vector3df& direction)
			{
				m_direction = direction;
				m_direction.normalize();
			}

			/** @brief Sets cylinder radius. */
			inline void setRadius(float r)
			{
				m_radius = r;
			}

			/** @brief Sets cylinder length. */
			inline void setLength(float l)
			{
				m_length = l;
			}

			/** @brief Gets axis direction. */
			inline const core::vector3df& getDirection()
			{
				return m_direction;
			}

			/** @brief Gets radius. */
			inline float getRadius()
			{
				return m_radius;
			}

			/** @brief Gets length. */
			inline float getLength()
			{
				return m_length;
			}

			/** @brief Implementation: spawns within volume or on surface. */
			virtual void generatePosition(CParticle& particle, bool full, CGroup* group);

			/** @brief Implementation: computes surface normal. */
			virtual core::vector3df computeNormal(const core::vector3df& point, CGroup* group);

			DECLARE_GETTYPENAME(CCylinder)
		};
	}
}