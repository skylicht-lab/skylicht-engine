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

#include "CParticle.h"
#include "Entity/CEntityPrefab.h"

#include "Emitters/CEmitter.h"
#include "Zones/CZone.h"
#include "Renderers/IRenderer.h"
#include "Systems/ISystem.h"

#include "Systems/CParticleSystem.h"
#include "Systems/CParticleInstancingSystem.h"
#include "Systems/CParticleCPUBufferSystem.h"

#include "CParticleInstancing.h"
#include "CParticleCPUBuffer.h"

#include "CModel.h"

#include "CParticleSerializable.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @struct SLaunchParticle
		 * @ingroup ParticleSystem
		 * @brief Information about particles being launched by an emitter.
		 */
		struct SLaunchParticle
		{
			CEmitter* Emitter;
			u32 Number;
			s32 Parent;

			SLaunchParticle()
			{
				Emitter = NULL;
				Number = 0;
				Parent = -1;
			}
		};

		/**
		 * @class IParticleCallback
		 * @ingroup ParticleSystem
		 * @brief Interface for receiving particle lifecycle events.
		 */
		class COMPONENT_API IParticleCallback
		{
		public:
			IParticleCallback()
			{

			}

			virtual ~IParticleCallback()
			{

			}

			virtual void OnParticleUpdate(CParticle* particles, int num, CGroup* group, float dt)
			{

			}

			virtual void OnParticleBorn(CParticle& p)
			{

			}

			virtual void OnParticleDead(CParticle& p)
			{

			}

			virtual void OnSwapParticleData(CParticle& p1, CParticle& p2)
			{

			}

			virtual void OnGroupDestroy()
			{

			}
		};

		/**
		 * @class CGroup
		 * @ingroup ParticleSystem
		 * @brief Represents a group of particles with shared settings, emitters, and a renderer.
		 * @details Groups manage the lifecycle of particles, their properties (life, friction, gravity), and the systems that affect them.
		 * 
		 * ### Example
		 * @code
		 * Particle::CGroup *group = ps->createParticleGroup();
		 * group->LifeMin = 0.5f;
		 * group->LifeMax = 1.0f;
		 * group->Gravity.set(0.0f, -10.0f, 0.0f);
		 * group->Friction = 0.5f;
		 * 
		 * // Setup color over life
		 * group->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
		 * @endcode
		 */
		class COMPONENT_API CGroup : public CParticleSerializable
		{
		protected:
			/** @brief Active particles in this group. */
			core::array<CParticle> m_particles;
			/** @brief Buffer of particles to be launched in the current frame. */
			core::array<SLaunchParticle> m_launch;

			/** @brief List of emitters assigned to this group. */
			std::vector<CEmitter*> m_emitters;
			/** @brief Custom update systems affecting this group's particles. */
			std::vector<ISystem*> m_systems;
			/** @brief Models defining parameter animation (e.g. ColorA over life). */
			std::vector<CModel*> m_models;
			/** @brief Interpolators shared by models. */
			std::vector<CInterpolator*> m_interpolators;

			/** @brief Core particle movement and lifecycle system. */
			CParticleSystem* m_particleSystem;

			/** @brief Internal system for GPU instancing updates. */
			CParticleInstancingSystem* m_instancingSystem;
			/** @brief Internal system for CPU buffer updates. */
			CParticleCPUBufferSystem* m_cpuBufferSystem;

			/** @brief The renderer used to draw this group. */
			IRenderer* m_renderer;

			/** @brief Buffer for GPU instancing data. */
			CParticleInstancing* m_instancing;
			/** @brief Buffer for CPU rendering data. */
			CParticleCPUBuffer* m_cpuBuffer;

			/** @brief Callbacks for particle lifecycle events. */
			std::vector<IParticleCallback*> m_callback;

		public:
			/** @brief Global friction for all particles in this group. */
			float Friction;
			/** @brief Minimum life time for newly born particles. */
			float LifeMin;
			/** @brief Maximum life time for newly born particles. */
			float LifeMax;
			/** @brief If true, particles do not die from age. */
			bool Immortal;

			/** @brief Gravity magnitude. */
			float GravityValue;
			/** @brief Gravity rotation (euler angles). */
			core::vector3df GravityRotation;
			/** @brief Global rotation for all particles. */
			core::vector3df ParticleRotation;

			/** @brief Calculated gravity vector. */
			core::vector3df Gravity;
			/** @brief Custom orientation normal for fixed billboarding. */
			core::vector3df OrientationNormal;
			/** @brief Custom orientation up vector for fixed billboarding. */
			core::vector3df OrientationUp;
			/** @brief Whether to use custom orientation instead of camera billboard. */
			bool UseOrientationAsBillboard;

			/** @brief Friendly name of the group. */
			std::wstring Name;
			/** @brief Visibility flag. */
			bool Visible;

			/** @brief If true, uses optimized particle swap on deletion (may affect sorting). */
			bool Optimized;

		protected:
			core::matrix4 m_parentWorld;
			core::matrix4 m_world;
			core::aabbox3df m_bbox;

			int m_frameUpdate;
		public:
			CGroup();

			virtual ~CGroup();

			/** @brief Clears all active particles. */
			void clearParticles();

			/** @brief Clears only immortal particles. */
			void clearImmortalParticles();

			/** @brief Main update loop for particle physics and lifecycle. */
			void update();

			/** @brief Updates buffers for rendering. */
			void updateForRenderer();

			/** @brief Sets gravity direction using euler rotation. */
			void setGravityRotation(const core::vector3df& euler);

			/** @brief Sets particle orientation using euler rotation. */
			void setParticleRotation(const core::vector3df& euler);

			/** @brief Sets the parent entity's world matrix. */
			inline void setParentWorldMatrix(const core::matrix4& m)
			{
				m_parentWorld = m;
			}

			/** @brief Sets the local-to-world transform of this group. */
			inline void setWorldMatrix(const core::matrix4& m)
			{
				m_world = m;
			}

			/** @brief Gets the world matrix of this group. */
			inline const core::matrix4& getWorld()
			{
				return m_world;
			}

			/** @brief Transforms a position from local group space to world space. */
			virtual core::vector3df getTransformPosition(const core::vector3df& pos);

			/** @brief Transforms a vector (rotation only) from local group space to world space. */
			virtual core::vector3df getTransformVector(const core::vector3df& vec);

			/** @brief Gets the bounding box of all active particles. */
			inline const core::aabbox3df& getBBox()
			{
				return m_bbox;
			}

			/** @brief Gets current particle count. */
			inline u32 getNumParticles()
			{
				return m_particles.size();
			}

			/** @brief Gets raw pointer to particle array. */
			inline CParticle* getParticlePointer()
			{
				return m_particles.pointer();
			}

			/** @brief Adds an emitter to the group. */
			inline CEmitter* addEmitter(CEmitter* e)
			{
				m_emitters.push_back(e);
				return e;
			}

			/** @brief Gets all emitters. */
			inline std::vector<CEmitter*>& getEmitters()
			{
				return m_emitters;
			}

			/** @brief Removes an emitter. */
			void removeEmitter(CEmitter* e);

			/** @brief Adds a lifecycle callback. */
			void addCallback(IParticleCallback* cb)
			{
				std::vector<IParticleCallback*>::iterator i = std::find(m_callback.begin(), m_callback.end(), cb);
				if (i == m_callback.end())
					m_callback.push_back(cb);
			}

			/** @brief Removes a lifecycle callback. */
			void removeCallback(IParticleCallback* cb)
			{
				std::vector<IParticleCallback*>::iterator i = std::find(m_callback.begin(), m_callback.end(), cb);
				if (i != m_callback.end())
					m_callback.erase(i);
			}

			/** @brief Gets all callbacks. */
			inline std::vector<IParticleCallback*>& getCallback()
			{
				return m_callback;
			}

			/** @brief Adds a custom system to process particles. */
			inline void addSystem(ISystem* s)
			{
				m_systems.push_back(s);
			}

			/** @brief Gets all systems. */
			inline std::vector<ISystem*>& getSystems()
			{
				return m_systems;
			}

			/** @brief Removes a system. */
			void removeSystem(ISystem* s)
			{
				std::vector<ISystem*>::iterator i = std::find(m_systems.begin(), m_systems.end(), s);
				if (i != m_systems.end())
					m_systems.erase(i);
			}

			/** @brief Manually spawns a particle using an emitter index. */
			int addParticle(u32 emiterID, const core::vector3df& position, const core::vector3df& subEmitterDirection)
			{
				if (emiterID < m_emitters.size())
				{
					return addParticleByEmitter(m_emitters[emiterID], position, subEmitterDirection);
				}

				return -1;
			}

			/** @brief Manually spawns a particle with initial velocity using an emitter index. */
			int addParticleVelocity(u32 emiterID, const core::vector3df& position, const core::vector3df& velocity)
			{
				if (emiterID < m_emitters.size())
				{
					return addParticleVelocityByEmitter(m_emitters[emiterID], position, velocity);
				}

				return -1;
			}

			/** @brief Manually spawns a particle using a pointer to an emitter. */
			int addParticleByEmitter(CEmitter* emitter, const core::vector3df& position, const core::vector3df& subEmitterDirection);

			/** @brief Manually spawns a particle with initial velocity using a pointer to an emitter. */
			int addParticleVelocityByEmitter(CEmitter* emitter, const core::vector3df& position, const core::vector3df& velocity);

			/** @brief Assigns a renderer to this group. */
			IRenderer* setRenderer(IRenderer* r);

			/** @brief Gets the current renderer. */
			inline IRenderer* getRenderer()
			{
				return m_renderer;
			}

			/** @brief Internal: gets instancing buffer. */
			CParticleInstancing* getIntancing()
			{
				return m_instancing;
			}

			/** @brief Internal: gets CPU mesh buffer. */
			CParticleCPUBuffer* getParticleBuffer()
			{
				return m_cpuBuffer;
			}

			/** @brief Gets current particle count. */
			inline u32 getCurrentParticleCount()
			{
				return m_particles.size();
			}

			/** @brief Creates or retrieves a model for a specific parameter name. */
			CModel* createModel(const std::wstring& attributeName);

			/** @brief Creates or retrieves a model for a specific parameter type. */
			CModel* createModel(EParticleParams param);

			/** @brief Finds a model for a parameter type. */
			CModel* getModel(EParticleParams param);

			/** @brief Deletes a model. */
			void deleteModel(EParticleParams param);

			/** @brief Gets all models. */
			std::vector<CModel*>& getModels()
			{
				return m_models;
			}

			/** @brief Creates a new interpolator. */
			CInterpolator* createInterpolator();

			/** @brief Deletes an interpolator. */
			void deleteInterpolator(CInterpolator* interpolator);

			/** @brief Gets all interpolators. */
			std::vector<CInterpolator*>& getInterpolators()
			{
				return m_interpolators;
			}

			/** @brief Gets raw access to particle list. */
			core::array<CParticle>& getParticles()
			{
				return m_particles;
			}

			/** @brief Gets last frame index update. */
			inline int getFrameUpdate()
			{
				return m_frameUpdate;
			}

			/** @brief Syncs frame count. */
			inline void updateFrame(int frame)
			{
				// sync frame from CParticleComponent
				m_frameUpdate = frame;
			}

			DECLARE_GETTYPENAME(CGroup)

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

		protected:

			virtual void updateLaunchEmitter();

			virtual void bornParticle();

			virtual bool launchParticle(CParticle& p, SLaunchParticle& launch);

			void initParticleModel(CParticle& p);

			inline void initParticleLifeTime(CParticle& p)
			{
				p.Age = 0.0f;
				p.Life = random(LifeMin, LifeMax);
				p.Immortal = Immortal;
				p.LifeTime = p.Life;
				p.HaveRotate = false;
			}

			CParticle* create(u32 num);

			void remove(u32 i);
		};
	}
}