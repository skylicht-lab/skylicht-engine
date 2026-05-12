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
		 * @class CRing
		 * @ingroup ParticleSystem
		 * @brief A zone that spawns particles within a 2D ring (annulus) in 3D space.
		 */
		class COMPONENT_API CRing : public CPositionZone
		{
		protected:
			/** @brief Normal vector defining the ring plane. */
			core::vector3df m_normal;
			/** @brief Inner radius. */
			float m_minRadius;
			/** @brief Outer radius. */
			float m_maxRadius;

		public:
			CRing(const core::vector3df& position, const core::vector3df& normal, float minRadius, float maxRadius);

			virtual ~CRing();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Sets plane orientation normal. */
			inline void setNormal(const core::vector3df& d)
			{
				m_normal = d;
				m_normal.normalize();
			}

			/** @brief Gets plane orientation normal. */
			inline core::vector3df& getNormal()
			{
				return m_normal;
			}

			/** @brief Sets inner and outer radius range. */
			void setRadius(float min, float max);

			/** @brief Gets inner radius. */
			inline float getMinRadius()
			{
				return m_minRadius;
			}

			/** @brief Gets outer radius. */
			inline float getMaxRadius()
			{
				return m_maxRadius;
			}

			/** @brief Implementation: spawns randomly between min and max radius on the plane. */
			virtual void generatePosition(CParticle& particle, bool full, CGroup* group);

			/** @brief Implementation: normal is the plane normal. */
			virtual core::vector3df computeNormal(const core::vector3df& point, CGroup* group);

			DECLARE_GETTYPENAME(CRing)
		};
	}
}