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

#include "CEmitter.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CDirectionEmitter
		 * @ingroup ParticleSystem
		 * @brief Base class for emitters that have a primary emission direction.
		 */
		class COMPONENT_API CDirectionEmitter : public CEmitter
		{
		protected:
			/** @brief Fixed orientation for emission. */
			core::quaternion m_rotation;
			/** @brief Main emission direction. */
			core::vector3df m_direction;

		public:
			CDirectionEmitter(EEmitter type);

			virtual ~CDirectionEmitter();

			virtual bool haveDirection()
			{
				return true;
			}

			/** @brief Sets emission rotation and updates direction vector. */
			virtual void setRotation(const core::quaternion& rotation);

			/** @brief Gets current emission rotation. */
			virtual const core::quaternion& getRotation()
			{
				return m_rotation;
			}

			/** @brief Sets emission direction vector and optionally updates rotation. */
			virtual void setDirection(const core::vector3df& d, bool updateRotation = true);

			/** @brief Gets current emission direction vector. */
			inline const core::vector3df& getDirection()
			{
				return m_direction;
			}
		};
	}
}