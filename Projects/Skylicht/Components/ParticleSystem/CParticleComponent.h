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
#include "Particles/CFactory.h"
#include "CParticleBufferData.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CParticleComponent
		 * @ingroup ParticleSystem
		 * @brief Main component for creating and managing a particle system in Skylicht.
		 * @details This component integrates the particle engine with the ECS architecture. 
		 * It manages particle groups, emitters, and zones, and handles serialization to .particle files.
		 * 
		 * ### Example
		 * @code
		 * // Create a particle system object
		 * CGameObject *psObj = zone->createEmptyObject();
		 * Particle::CParticleComponent *ps = psObj->addComponent<Particle::CParticleComponent>();
		 * 
		 * // Setup a basic group with a quad renderer
		 * Particle::CGroup *group = ps->createParticleGroup();
		 * group->LifeMin = 1.0f;
		 * group->LifeMax = 2.0f;
		 * 
		 * Particle::CFactory *factory = ps->getParticleFactory();
		 * Particle::CQuadRenderer *renderer = factory->createQuadRenderer();
		 * renderer->setTexturePath("Particles/Textures/point.png");
		 * group->setRenderer(renderer);
		 * 
		 * // Setup a random emitter in a sphere zone
		 * Particle::CEmitter *emitter = group->addEmitter(factory->createRandomEmitter());
		 * emitter->setZone(factory->createSphereZone(core::vector3df(0,0,0), 1.0f));
		 * emitter->setFlow(100.0f);
		 * emitter->setForce(0.1f, 0.5f);
		 * @endcode
		 */
		class COMPONENT_API CParticleComponent : public CComponentSystem
		{
		protected:
			/**
			 * @brief Cached pointer to the ECS entity data.
			 */
			CParticleBufferData* m_data;

			/**
			 * @brief Internal factory for creating particle objects.
			 */
			CFactory m_factory;

			/**
			 * @brief Path to the .particle source file.
			 */
			std::string m_sourcePath;

			/**
			 * @brief Frame counter for update synchronization.
			 */
			int m_frameUpdate;

		public:
			CParticleComponent();

			virtual ~CParticleComponent();

			/**
			 * @brief Initializes component data and required ECS systems.
			 */
			virtual void initComponent();

			/**
			 * @brief Starts the component, loading the particle source if set.
			 */
			virtual void startComponent();

			/**
			 * @brief Updates the internal frame counter.
			 */
			virtual void updateComponent();

			/**
			 * @brief Creates a serializable object for property editing.
			 */
			virtual CObjectSerializable* createSerializable();

			/**
			 * @brief Loads properties from a serializable object.
			 */
			virtual void loadSerializable(CObjectSerializable* object);

		public:

			/**
			 * @brief Gets the internal particle factory.
			 * @return Pointer to CFactory.
			 */
			CFactory* getParticleFactory()
			{
				return &m_factory;
			}

			/**
			 * @brief Gets the ECS entity data holding particle groups.
			 * @return Pointer to CParticleBufferData.
			 */
			inline CParticleBufferData* getData()
			{
				return m_data;
			}

			/**
			 * @brief Updates the transform of all particle groups.
			 * @param world The new world matrix.
			 */
			void setGroupTransform(const core::matrix4& world);

			/**
			 * @brief Creates a new top-level particle group managed by this component.
			 * @return Pointer to the new CGroup.
			 */
			CGroup* createParticleGroup();

			/**
			 * @brief Creates a sub-group attached to a parent group.
			 * @param group The parent group.
			 * @return Pointer to the new CSubGroup.
			 */
			CSubGroup* createParticleSubGroup(CGroup* group);

			/**
			 * @brief Gets the number of top-level groups.
			 */
			inline u32 getNumOfGroup()
			{
				return m_data->Groups.size();
			}

			/**
			 * @brief Retrieves a specific group by index.
			 */
			inline CGroup* getGroup(int i)
			{
				return m_data->Groups[i];
			}

			/**
			 * @brief Removes a specific group from the component.
			 */
			void removeParticleGroup(CGroup* group);

			/**
			 * @brief Dynamically adjusts the emission rate of all emitters.
			 * @param f Scaling factor (0.0 to 1.0).
			 * @param includeSubGroup Whether to affect sub-groups.
			 */
			void updateParticleCountByPercentage(float f, bool includeSubGroup = false);

			/**
			 * @brief Resets all emitters and starts playback.
			 */
			void Play();

			/**
			 * @brief Stops all emitters and clears immortal particles.
			 */
			void Stop();

			/**
			 * @brief Checks if any particle is currently alive.
			 */
			bool IsPlaying();

			/**
			 * @brief Immediately removes all active particles across all groups.
			 */
			void clearParticles();

			/**
			 * @brief Gets the total count of active particles in all groups.
			 */
			u32 getTotalParticle();

			/**
			 * @brief Gets the current .particle file path.
			 */
			inline const char* getSourcePath()
			{
				return m_sourcePath.c_str();
			}

			/**
			 * @brief Sets the .particle file path.
			 */
			inline void setSourcePath(const char* path)
			{
				m_sourcePath = path;
			}

			/**
			 * @brief Loads the particle system configuration from m_sourcePath.
			 * @return True if successful.
			 */
			bool load();

			/**
			 * @brief Saves the current particle system configuration to m_sourcePath.
			 * @return True if successful.
			 */
			bool save();

			/**
			 * @brief Gets the current frame update count.
			 */
			inline int getFrameUpdated()
			{
				return m_frameUpdate;
			}

			/**
			 * @brief Clones a particle group.
			 */
			CGroup* duplicateGroup(CGroup* group);

			/**
			 * @brief Clones an emitter within a group.
			 */
			CEmitter* duplicateEmitter(CGroup* group, CEmitter* emitter);

			DECLARE_GETTYPENAME(CParticleComponent)

		protected:

			/**
			 * @brief Internal serialization helper.
			 */
			void saveGroups(CObjectSerializable* groups);

			/**
			 * @brief Internal serialization helper for a specific group.
			 */
			void saveGroup(Particle::CGroup* group, CObjectSerializable* object);

			/**
			 * @brief Internal XML loading helper for a specific group.
			 */
			void loadGroup(Particle::CGroup* group, io::IXMLReader* reader);

			/**
			 * @brief Internal serialization loading helper for a specific group.
			 */
			void loadGroup(Particle::CGroup* group, CObjectSerializable* object);

			/**
			 * @brief Internal XML loading helper for emitters.
			 */
			void loadEmitters(Particle::CGroup* group, io::IXMLReader* reader);

			/**
			 * @brief Internal XML loading helper for models.
			 */
			void loadModels(Particle::CGroup* group, io::IXMLReader* reader);

			/**
			 * @brief Internal XML loading helper for renderers.
			 */
			void loadRenderer(Particle::CGroup* group, io::IXMLReader* reader);

			/**
			 * @brief Internal XML loading helper for sub-groups.
			 */
			void loadSubGroups(Particle::CGroup* group, io::IXMLReader* reader);
		};
	}
}