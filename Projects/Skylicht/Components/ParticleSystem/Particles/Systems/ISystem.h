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

namespace Skylicht
{
	namespace Particle
	{
		class CParticle;
		class CGroup;

		/**
		 * @class ISystem
		 * @ingroup ParticleSystem
		 * @brief Base interface for particle systems that update particle data.
		 * @details Systems are responsible for iterating over active particles and applying logic (physics, effects, buffer updates).
		 */
		class COMPONENT_API ISystem
		{
		protected:
			/** @brief Whether the system is active. */
			bool m_enable;

		public:
			ISystem() :
				m_enable(true)
			{

			}

			virtual ~ISystem()
			{

			}

			/** @brief Core update logic.
			 * @param particles Pointer to the particle array.
			 * @param num Number of active particles.
			 * @param group The owner group.
			 * @param dt Delta time in milliseconds.
			 */
			virtual void update(CParticle *particles, int num, CGroup *group, float dt) = 0;

			/** @brief Enables or disables the system. */
			inline void setEnable(bool b)
			{
				m_enable = b;
			}

			/** @brief Checks if the system is enabled. */
			inline bool isEnable()
			{
				return m_enable;
			}
		};
	}
}