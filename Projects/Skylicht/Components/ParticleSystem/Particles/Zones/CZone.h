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

#include "ParticleSystem/Particles/CParticleSerializable.h"

namespace Skylicht
{
	namespace Particle
	{
		class CParticle;
		class CGroup;

		COMPONENT_API int random(int from, int to);

		COMPONENT_API float random(float from, float to);

		COMPONENT_API void random_reset(s32 seed);

		/**
		 * @enum EZone
		 * @ingroup ParticleSystem
		 * @brief Available particle spawn zones.
		 */
		enum EZone
		{
			Point,
			Sphere,
			AABox,
			Cylinder,
			Line,
			PolyLine,
			Ring,
			NumOfZone
		};

		/**
		 * @class CZone
		 * @ingroup ParticleSystem
		 * @brief Base class for zones where particles can be spawned.
		 */
		class COMPONENT_API CZone : public CParticleSerializable
		{
		protected:
			EZone m_type;

		public:
			CZone(EZone type);

			virtual ~CZone();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Get the display name of the zone type.
			 * @return Wstring identifier.
			 */
			const wchar_t* getName();
			
			/**
			 * @brief Get the zone type ID.
			 * @return EZone enum.
			 */
			inline EZone getType()
			{
				return m_type;
			}

			/**
			 * @brief Normalize a vector, or randomize it if it's zero-length.
			 * @param v Vector to process.
			 */
			void normalizeOrRandomize(core::vector3df& v);

			/**
			 * @brief Generate a spawn position for a particle within this zone.
			 * @param particle Target particle.
			 * @param full If true, spawns anywhere in the volume; if false, usually spawns on the surface.
			 * @param group Parent group.
			 */
			virtual void generatePosition(CParticle& particle, bool full, CGroup* group) = 0;

			/**
			 * @brief Compute the surface normal at a given point relative to the zone.
			 * @param point The point in world space.
			 * @param group Parent group.
			 * @return Normal vector.
			 */
			virtual core::vector3df computeNormal(const core::vector3df& point, CGroup* group) = 0;

			DECLARE_GETTYPENAME(CZone)
		};
	}
}