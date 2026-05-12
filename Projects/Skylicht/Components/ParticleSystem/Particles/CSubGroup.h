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

#include "CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CSubGroup
		 * @ingroup ParticleSystem
		 * @brief A particle group that spawns particles from the positions of particles in a parent group.
		 * @details Useful for sub-emitter effects like trails, explosions upon death, or complex multi-stage particles.
		 * 
		 * ### Example
		 * @code
		 * // Create a sub-group attached to projectileGroup
		 * Particle::CSubGroup *arcaneGroup = ps->createParticleSubGroup(projectileGroup);
		 * arcaneGroup->setFollowParentTransform(true);
		 * arcaneGroup->syncParentParams(true, true); // Sync life and color
		 * @endcode
		 */
		class COMPONENT_API CSubGroup :
			public CGroup,
			public IParticleCallback
		{
		protected:
			/** @brief Pointer to the parent particle group. */
			CGroup* m_parentGroup;

			/** @brief System that syncs this group's particles to parent particles. */
			ISystem* m_parentSystem;

			/** @brief Spawn position (derived from parent particle). */
			core::vector3df m_position;
			/** @brief Spawn direction (derived from parent velocity). */
			core::vector3df m_direction;
			/** @brief Spawn rotation (derived from parent orientation). */
			core::quaternion m_rotate;

			/** @brief Whether particles should follow the parent's current position every frame. */
			bool m_followParentTransform;
			/** @brief Whether emitter orientation should be in world space or relative to parent. */
			bool m_emitterWorldOrientation;
			/** @brief Whether to sync child age/life with parent. */
			bool m_syncLife;
			/** @brief Whether to sync child color with parent. */
			bool m_syncColor;

		public:
			CSubGroup(CGroup* group);

			virtual ~CSubGroup();

			/** @brief Initializes internal born data for all emitters based on current parent particles. */
			void initParticles();

			/** @brief Callback when a parent particle is born. */
			virtual void OnParticleBorn(CParticle& p);

			/** @brief Callback when a parent particle dies. */
			virtual void OnParticleDead(CParticle& p);

			/** @brief Callback when parent particle data is swapped in the array. */
			virtual void OnSwapParticleData(CParticle& p1, CParticle& p2);

			/** @brief Callback when the parent group is destroyed. */
			virtual void OnGroupDestroy();

			/** @brief Internal: updates launch emitters based on parent particles. */
			virtual void updateLaunchEmitter();

			/** @brief Internal: spawns new particles at parent positions. */
			virtual void bornParticle();

			/** @brief Transforms position from parent local space to world space. */
			virtual core::vector3df getTransformPosition(const core::vector3df& pos);

			/** @brief Transforms vector from parent local space to world space. */
			virtual core::vector3df getTransformVector(const core::vector3df& vec);

			/** @brief Enables/disables following the parent's movement. */
			inline void setFollowParentTransform(bool b)
			{
				m_followParentTransform = b;
				m_parentSystem->setEnable(b);
			}

			/** @brief Sets emitter orientation mode. */
			inline void setEmitterWorldOrientation(bool b)
			{
				m_emitterWorldOrientation = b;
			}

			/** @brief Configures synchronization of life and color from parent. */
			void syncParentParams(bool life, bool color);

			/** @brief Gets the parent group. */
			inline CGroup* getParentGroup()
			{
				return m_parentGroup;
			}

			DECLARE_GETTYPENAME(CSubGroup)

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);
		};
	}
}