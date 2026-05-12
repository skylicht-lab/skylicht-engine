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
		/**
		 * @struct SParticleInstance
		 * @ingroup ParticleSystem
		 * @brief GPU instance data for hardware-accelerated particles.
		 * @details This structure is passed to the GPU for each particle when using CQuadRenderer or CMeshParticleRenderer.
		 */
		struct SParticleInstance
		{
			/** @brief World position. */
			video::SVec4 Pos;
			/** @brief Tint color. */
			SColor Color;
			/** @brief Scale for texture atlas frames. */
			core::vector2df UVScale;
			/** @brief Offset for texture atlas frames. */
			core::vector2df UVOffset;
			/** @brief World size. */
			video::SVec4 Size;
			/** @brief World rotation (Euler). */
			video::SVec4 Rotation;
			/** @brief Movement velocity. */
			video::SVec4 Velocity;

			bool operator==(const SParticleInstance& other) const
			{
				return Pos == other.Pos
					&& Size == other.Size
					&& Rotation == other.Rotation
					&& Color == other.Color
					&& UVScale == other.UVScale
					&& UVOffset == other.UVOffset
					&& Velocity == other.Velocity;
			}
		};

		/**
		 * @class CParticleInstancing
		 * @ingroup ParticleSystem
		 * @brief Internal class for managing GPU instancing buffers for particles.
		 * @details Manages a vertex buffer containing SParticleInstance data.
		 */
		class COMPONENT_API CParticleInstancing
		{
		protected:
			/** @brief The base mesh buffer (e.g. single quad or mesh). */
			IMeshBuffer* m_meshBuffer;
			/** @brief The dynamic vertex buffer containing instance data. */
			CVertexBuffer<SParticleInstance>* m_instanceBuffer;

		public:
			CParticleInstancing();

			virtual ~CParticleInstancing();

			/** @brief Gets the base geometry mesh buffer. */
			inline IMeshBuffer* getMeshBuffer()
			{
				return m_meshBuffer;
			}

			/** @brief Gets the instance data buffer. */
			inline CVertexBuffer<SParticleInstance>* getInstanceBuffer()
			{
				return m_instanceBuffer;
			}
		};
	}
}