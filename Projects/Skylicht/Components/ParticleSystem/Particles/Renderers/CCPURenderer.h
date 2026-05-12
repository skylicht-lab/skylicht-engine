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

#include "IRenderer.h"
#include "ParticleSystem/Particles/CParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CCPURenderer
		 * @ingroup ParticleSystem
		 * @brief A non-instanced renderer that builds the particle vertex buffer on the CPU every frame.
		 * @details Used for compatibility or when instancing is not supported.
		 */
		class COMPONENT_API CCPURenderer : public IRenderer
		{
		public:
			/**
			 * @enum EBillboardType
			 * @brief Orientation modes for particles generated on CPU.
			 */
			enum EBillboardType
			{
				Billboard,	/**< Faces camera. */
				Frontal,	/**< Oriented by velocity vector. */
				Velocity,	/**< Faces camera but aligned to velocity. */
				RotateY		/**< Faces camera but fixed to Y axis. */
			};

			/**
			 * @enum ETransparentType
			 * @brief Blending modes.
			 */
			enum ETransparentType
			{
				Addtive,	/**< Additive blending (Additive.xml). */
				Transparent	/**< Standard alpha blending (Transparent.xml). */
			};

		protected:
			static const u32 NB_INDICES_PER_QUAD = 6;
			static const u32 NB_VERTICES_PER_QUAD = 4;

			EBillboardType m_billboardType;
			ETransparentType m_transparentType;

		public:
			CCPURenderer();

			virtual ~CCPURenderer();

			/** @brief Prepares the mesh buffer for CPU updates. */
			virtual void getParticleBuffer(IMeshBuffer* buffer);

			/** @brief Rebuilds the entire vertex buffer on the CPU based on current particle positions. */
			virtual void updateParticleBuffer(IMeshBuffer* buffer, CParticle* particles, int num);

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Gets current billboarding mode. */
			inline EBillboardType getBillboardType()
			{
				return m_billboardType;
			}

			/** @brief Sets current billboarding mode. */
			inline void setBillboardType(EBillboardType type)
			{
				m_billboardType = type;
			}

			DECLARE_GETTYPENAME(CCPURenderer)
		};
	}
}