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
		class CVortexSystem : public ISystem
		{
		protected:
			core::vector3df m_position;
			core::vector3df m_direction;
			float m_rotationSpeed;
			float m_attractionSpeed;
			float m_eyeAttractionSpeed;
			float m_eyeRadius;
			bool m_killingParticleEnabled;

		public:
			CVortexSystem(const core::vector3df& position, const core::vector3df& direction, float rotationSpeed = 1.0f, float attractionSpeed = 0.0f);

			virtual ~CVortexSystem();

			virtual void update(CParticle *particles, int num, CGroup *group, float dt);

			inline core::vector3df getPosition()
			{
				return m_position;
			}

			inline core::vector3df getDirection()
			{
				return m_direction;
			}

			inline void setPosition(const core::vector3df& p)
			{
				m_position = p;
			}

			inline void setDirection(const core::vector3df& d)
			{
				m_direction = d;
			}

			inline float getRotateSpeed()
			{
				return m_rotationSpeed;
			}

			inline float getAttractionSpeed()
			{
				return m_attractionSpeed;
			}

			inline float getEyeRadius()
			{
				return m_eyeRadius;
			}

			inline void setRotateSpeed(float f)
			{
				m_rotationSpeed = f;
			}

			inline void setAttractionSpeed(float f)
			{
				m_attractionSpeed = f;
			}

			inline void setEyeRadius(float f)
			{
				m_eyeRadius = f;
			}

			inline void setEyeAttractionSpeed(float f)
			{
				m_eyeAttractionSpeed = f;
			}

			inline float getEyeAttractionSpeed()
			{
				return m_eyeAttractionSpeed;
			}

			inline bool isKillingParticle()
			{
				return m_killingParticleEnabled;
			}

			inline void enableKillingParticle(bool b)
			{
				m_killingParticleEnabled = b;
			}
		};
	}
}