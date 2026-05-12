/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CParticleSerializable
		 * @ingroup ParticleSystem
		 * @brief Base class for particle system objects that can be serialized to XML.
		 * @details Provides the virtual interface for creating and loading serializable data.
		 */
		class COMPONENT_API CParticleSerializable
		{
		public:
			CParticleSerializable();

			virtual ~CParticleSerializable();

			/**
			 * @brief Creates a serializable object for property editing or saving.
			 * @return Pointer to a new CObjectSerializable instance.
			 */
			virtual CObjectSerializable* createSerializable();

			/**
			 * @brief Loads properties from a serializable object.
			 * @param object The serializable data to load.
			 */
			virtual void loadSerializable(CObjectSerializable* object);

			DECLARE_GETTYPENAME(CParticleSerializable)
		};
	}
}