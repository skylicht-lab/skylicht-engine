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

#include "Components/CComponentSystem.h"
#include "CParticleTrailData.h"
#include "CTrailSerializable.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CParticleTrailComponent
		 * @ingroup ParticleSystem
		 * @brief Component for adding ribbon/trail effects to particles.
		 * @details Connects to a CParticleComponent to generate smooth trails based on particle movement.
		 * 
		 * ### Example
		 * @code
		 * // Assuming ps is a CParticleComponent already attached to the object
		 * Particle::CParticleTrailComponent *psTrail = psObj->addComponent<Particle::CParticleTrailComponent>();
		 * 
		 * // Add a trail to a specific particle group
		 * Particle::CParticleTrail *trail = psTrail->addTrail(projectileGroup);
		 * trail->setTexturePath("Particles/Textures/arcane_trail.png");
		 * trail->setWidth(0.3f);
		 * trail->setLength(2.0f);
		 * trail->setEmission(true);
		 * @endcode
		 */
		class COMPONENT_API CParticleTrailComponent : public CComponentSystem
		{
		protected:
			/**
			 * @brief Cached pointer to the trail entity data.
			 */
			CParticleTrailData* m_data;

		public:
			CParticleTrailComponent();

			virtual ~CParticleTrailComponent();

			/**
			 * @brief Initializes component data and trail renderer system.
			 */
			virtual void initComponent();

			/**
			 * @brief Updates trail transforms based on parent entity world matrix.
			 */
			virtual void updateComponent();

			/**
			 * @brief Creates a serializable object for property editing.
			 */
			virtual CObjectSerializable* createSerializable();

			/**
			 * @brief Loads trail settings from a serializable object.
			 */
			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Adds a new trail effect linked to a specific particle group.
			 * @param group The target particle group.
			 * @return Pointer to the new CParticleTrail instance.
			 */
			CParticleTrail* addTrail(CGroup* group);

			DECLARE_GETTYPENAME(CParticleTrailComponent)
		};
	}
}