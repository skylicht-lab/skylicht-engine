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

#include "ISystem.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CVortexSystem
		 * @ingroup ParticleSystem
		 * @brief Custom system that applies a vortex (swirl) effect to particles.
		 * 
		 * ### Example
		 * @code
		 * Particle::CVortexSystem *vortex = new Particle::CVortexSystem(core::vector3df(0, 1, 0), core::vector3df(0, 1, 0), 0.5f, 0.2f);
		 * vortex->setEyeAttractionSpeed(0.01f);
		 * vortex->enableKillingParticle(true);
		 * group->addSystem(vortex);
		 * @endcode
		 */
		class COMPONENT_API CVortexSystem : public ISystem
		{
		protected:
			/** @brief Center of the vortex. */
			core::vector3df m_position;
			/** @brief Axis of rotation. */
			core::vector3df m_direction;
			/** @brief Speed of rotation around the axis. */
			float m_rotationSpeed;
			/** @brief Speed at which particles are pulled toward the axis. */
			float m_attractionSpeed;
			/** @brief Speed at which particles move along the axis. */
			float m_eyeAttractionSpeed;
			/** @brief Radius of the vortex core. */
			float m_eyeRadius;
			/** @brief Whether to kill particles that reach the core. */
			bool m_killingParticleEnabled;

		public:
			CVortexSystem(const core::vector3df& position, const core::vector3df& direction, float rotationSpeed = 1.0f, float attractionSpeed = 0.0f);

			virtual ~CVortexSystem();

			/** @brief Implementation: applies vortex forces to particles. */
			virtual void update(CParticle *particles, int num, CGroup *group, float dt);

			/** @brief Gets center position. */
			inline core::vector3df getPosition()
			{
				return m_position;
			}

			/** @brief Gets rotation axis. */
			inline core::vector3df getDirection()
			{
				return m_direction;
			}

			/** @brief Sets center position. */
			inline void setPosition(const core::vector3df& p)
			{
				m_position = p;
			}

			/** @brief Sets rotation axis. */
			inline void setDirection(const core::vector3df& d)
			{
				m_direction = d;
			}

			/** @brief Gets rotation speed. */
			inline float getRotateSpeed()
			{
				return m_rotationSpeed;
			}

			/** @brief Gets radial attraction speed. */
			inline float getAttractionSpeed()
			{
				return m_attractionSpeed;
			}

			/** @brief Gets core radius. */
			inline float getEyeRadius()
			{
				return m_eyeRadius;
			}

			/** @brief Sets rotation speed. */
			inline void setRotateSpeed(float f)
			{
				m_rotationSpeed = f;
			}

			/** @brief Sets radial attraction speed. */
			inline void setAttractionSpeed(float f)
			{
				m_attractionSpeed = f;
			}

			/** @brief Sets core radius. */
			inline void setEyeRadius(float f)
			{
				m_eyeRadius = f;
			}

			/** @brief Sets axial attraction speed. */
			inline void setEyeAttractionSpeed(float f)
			{
				m_eyeAttractionSpeed = f;
			}

			/** @brief Gets axial attraction speed. */
			inline float getEyeAttractionSpeed()
			{
				return m_eyeAttractionSpeed;
			}

			/** @brief Checks if killing at core is enabled. */
			inline bool isKillingParticle()
			{
				return m_killingParticleEnabled;
			}

			/** @brief Enables/disables killing particles at the core. */
			inline void enableKillingParticle(bool b)
			{
				m_killingParticleEnabled = b;
			}
		};
	}
}