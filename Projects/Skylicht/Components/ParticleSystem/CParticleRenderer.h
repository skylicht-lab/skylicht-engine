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

#include "Entity/IEntityData.h"
#include "Entity/IRenderSystem.h"
#include "Entity/CEntityGroup.h"

#include "Culling/CCullingBBoxData.h"
#include "Culling/CCullingData.h"
#include "Culling/CVisibleData.h"
#include "Transform/CWorldTransformData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "ParticleSystem/CParticleBufferData.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CParticleRenderer
		 * @ingroup ParticleSystem
		 * @brief ECS render system for particles.
		 * @details Handles the actual drawing of particles using the renderers assigned to each group.
		 * Supports standard, transparent, and emission passes.
		 */
		class COMPONENT_API CParticleRenderer : public IRenderSystem
		{
		protected:
			/** @brief Cached entity group. */
			CEntityGroup* m_group;
			/** @brief Internal scratchpad matrix. */
			core::matrix4 m_transform;

			/** @brief Current billboard up vector. */
			core::vector3df m_billboardUp;
			/** @brief Current billboard look vector. */
			core::vector3df m_billboardLook;
		public:
			CParticleRenderer();

			virtual ~CParticleRenderer();

			/** @brief ECS hook: establishes filters for entities with particle data. */
			virtual void beginQuery(CEntityManager* entityManager);

			/** @brief ECS hook: query implementation. */
			virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

			/** @brief ECS hook: initialization. */
			virtual void init(CEntityManager* entityManager);

			/** @brief ECS hook: per-frame updates. */
			virtual void update(CEntityManager* entityManager);

			/** @brief ECS hook: main render pass (standard/opaque). */
			virtual void render(CEntityManager* entityManager);

			/** @brief ECS hook: transparent render pass. */
			virtual void renderTransparent(CEntityManager* entityManager);

			/** @brief ECS hook: emission render pass. */
			virtual void renderEmission(CEntityManager* entityManager);

		protected:

			/** @brief Internal helper to calculate transform without rotation. */
			const core::matrix4& getTransformNoRotate(const core::matrix4& world);

			/** @brief Internal: renders all groups for an entity. */
			void renderParticleGroup(CParticleBufferData* data, const core::matrix4& world);

			/** @brief Internal: renders emission groups for an entity. */
			void renderParticleGroupEmission(CParticleBufferData* data, const core::matrix4& world);

			/** @brief Internal: submits a specific group to the video driver. */
			void renderGroup(IVideoDriver* driver, Particle::CGroup* group);
		};
	}
}