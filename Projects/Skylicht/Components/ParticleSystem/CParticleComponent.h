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
			CParticleBufferData* m_data;

			CFactory m_factory;

			std::string m_sourcePath;

			int m_frameUpdate;

		public:
			CParticleComponent();

			virtual ~CParticleComponent();

			virtual void initComponent();

			virtual void startComponent();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

		public:

			/**
			 * @brief Get the factory used to create particle emitters, zones, and renderers.
			 * @return Pointer to CFactory.
			 */
			CFactory* getParticleFactory()
			{
				return &m_factory;
			}

			/**
			 * @brief Get the internal ECS data structure holding particle groups.
			 * @return Pointer to CParticleBufferData.
			 */
			inline CParticleBufferData* getData()
			{
				return m_data;
			}

			/**
			 * @brief Set the world transform for all particle groups in this system.
			 * @param world The world matrix.
			 */
			void setGroupTransform(const core::matrix4& world);

			/**
			 * @brief Create a new particle group.
			 * @return Pointer to the new CGroup.
			 */
			CGroup* createParticleGroup();

			/**
			 * @brief Create a new sub-group that follows particles from a parent group.
			 * @param group Pointer to the parent group.
			 * @return Pointer to the new CSubGroup.
			 */
			CSubGroup* createParticleSubGroup(CGroup* group);

			/**
			 * @brief Get the number of particle groups.
			 * @return Count of groups.
			 */
			inline u32 getNumOfGroup()
			{
				return m_data->Groups.size();
			}

			/**
			 * @brief Get a particle group by index.
			 * @param i Index of the group.
			 * @return Pointer to CGroup.
			 */
			inline CGroup* getGroup(int i)
			{
				return m_data->Groups[i];
			}

			/**
			 * @brief Remove a particle group from the system.
			 * @param group Pointer to the group to remove.
			 */
			void removeParticleGroup(CGroup* group);

			/**
			 * @brief Scale the particle emission flow and tank values globally.
			 * @param f Scale factor (0.0 to 1.0).
			 * @param includeSubGroup Whether to include sub-groups in the update.
			 */
			void updateParticleCountByPercentage(float f, bool includeSubGroup = false);

			/**
			 * @brief Start particle emission for all groups.
			 */
			void Play();

			/**
			 * @brief Stop particle emission for all groups.
			 */
			void Stop();

			/**
			 * @brief Check if any group has active particles.
			 * @return True if particles are alive.
			 */
			bool IsPlaying();

			/**
			 * @brief Kill all active particles immediately.
			 */
			void clearParticles();

			/**
			 * @brief Get total count of active particles across all groups.
			 * @return Total particle count.
			 */
			u32 getTotalParticle();

			/**
			 * @brief Get the path to the .particle source file.
			 * @return File path string.
			 */
			inline const char* getSourcePath()
			{
				return m_sourcePath.c_str();
			}

			/**
			 * @brief Set the path to the .particle source file.
			 * @param path File path string.
			 */
			inline void setSourcePath(const char* path)
			{
				m_sourcePath = path;
			}

			/**
			 * @brief Load the particle system configuration from the source path.
			 * @return True if successful.
			 */
			bool load();

			/**
			 * @brief Save the particle system configuration to the source path.
			 * @return True if successful.
			 */
			bool save();

			/**
			 * @brief Get the current frame update counter.
			 * @return Frame index.
			 */
			inline int getFrameUpdated()
			{
				return m_frameUpdate;
			}

			/**
			 * @brief Duplicate an existing particle group within this system.
			 * @param group The group to duplicate.
			 * @return Pointer to the new CGroup.
			 */
			CGroup* duplicateGroup(CGroup* group);

			/**
			 * @brief Duplicate an existing emitter into a group.
			 * @param group Target group.
			 * @param emitter Emitter to duplicate.
			 * @return Pointer to the new CEmitter.
			 */
			CEmitter* duplicateEmitter(CGroup* group, CEmitter* emitter);

			DECLARE_GETTYPENAME(CParticleComponent)

		protected:

			void saveGroups(CObjectSerializable* groups);

			void saveGroup(Particle::CGroup* group, CObjectSerializable* object);

			void loadGroup(Particle::CGroup* group, io::IXMLReader* reader);

			void loadGroup(Particle::CGroup* group, CObjectSerializable* object);

			void loadEmitters(Particle::CGroup* group, io::IXMLReader* reader);

			void loadModels(Particle::CGroup* group, io::IXMLReader* reader);

			void loadRenderer(Particle::CGroup* group, io::IXMLReader* reader);

			void loadSubGroups(Particle::CGroup* group, io::IXMLReader* reader);
		};
	}
}