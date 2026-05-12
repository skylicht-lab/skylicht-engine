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

#include "CDirectionEmitter.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CSphericEmitter
		 * @ingroup ParticleSystem
		 * @brief Emitter that shoots particles in a cone shape defined by two angles.
		 * 
		 * ### Example
		 * @code
		 * Particle::CSphericEmitter *emitter = factory->createSphericEmitter(core::vector3df(0, 1, 0), 0.0f, 45.0f * core::DEGTORAD);
		 * group->addEmitter(emitter);
		 * @endcode
		 */
		class COMPONENT_API CSphericEmitter : public CDirectionEmitter
		{
		protected:
			/** @brief Minimum opening angle of the cone. */
			float m_angleA;
			/** @brief Maximum opening angle of the cone. */
			float m_angleB;
			float m_cosAngleMin;
			float m_cosAngleMax;

			/** @brief Local rotation matrix for direction calculation. */
			float m_matrix[9];
		public:
			CSphericEmitter();

			virtual ~CSphericEmitter();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Sets cone axis and updates orientation matrix. */
			virtual void setDirection(const core::vector3df& d, bool updateRotation = true);

			/** @brief Sets opening angle range for the emission cone. */
			void setAngles(float a, float b);

			/** @brief Gets min angle. */
			inline float getAngleA()
			{
				return m_angleA;
			}

			/** @brief Gets max angle. */
			inline float getAngleB()
			{
				return m_angleB;
			}

			/** @brief Implementation: generates velocity within the cone. */
			virtual void generateVelocity(CParticle& particle, float speed, CZone* zone, CGroup* group);

			DECLARE_GETTYPENAME(CSphericEmitter)
		};
	}
}