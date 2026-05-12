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
			core::array<CParticle> m_particles;
			core::array<SLaunchParticle> m_launch;

			std::vector<CEmitter*> m_emitters;
			std::vector<ISystem*> m_systems;
			std::vector<CModel*> m_models;
			std::vector<CInterpolator*> m_interpolators;

			CParticleSystem* m_particleSystem;

			CParticleInstancingSystem* m_instancingSystem;
			CParticleCPUBufferSystem* m_cpuBufferSystem;

			IRenderer* m_renderer;

			CParticleInstancing* m_instancing;
			CParticleCPUBuffer* m_cpuBuffer;

			std::vector<IParticleCallback*> m_callback;

		public:
			float Friction;
			float LifeMin;
			float LifeMax;
			bool Immortal;

			float GravityValue;
			core::vector3df GravityRotation;
			core::vector3df ParticleRotation;

			core::vector3df Gravity;
			core::vector3df OrientationNormal;
			core::vector3df OrientationUp;
			bool UseOrientationAsBillboard;

			std::wstring Name;
			bool Visible;

			bool Optimized;

		protected:
			core::matrix4 m_parentWorld;
			core::matrix4 m_world;
			core::aabbox3df m_bbox;

			int m_frameUpdate;
		public:
			CGroup();

			virtual ~CGroup();

			/**
			 * @brief Kill all active particles in this group immediately.
			 */
			void clearParticles();

			/**
			 * @brief Kill all immortal particles.
			 */
			void clearImmortalParticles();

			/**
			 * @brief Update particle physics and logic systems. Called by CParticleGroupSystem.
			 */
			void update();

			/**
			 * @brief Update vertex buffers for rendering. Called by CParticleRenderer or CGUIParticle.
			 */
			void updateForRenderer();

			/**
			 * @brief Set the rotation of the gravity vector.
			 * @param euler Euler angles in radians.
			 */
			void setGravityRotation(const core::vector3df& euler);

			/**
			 * @brief Set the fixed orientation for particles in this group.
			 * @param euler Euler angles in radians.
			 */
			void setParticleRotation(const core::vector3df& euler);

			/**
			 * @brief Set the world matrix of the parent entity.
			 * @param m Parent world matrix.
			 */
			inline void setParentWorldMatrix(const core::matrix4& m)
			{
				m_parentWorld = m;
			}

			/**
			 * @brief Set the local-to-world matrix for this group.
			 * @param m World matrix.
			 */
			inline void setWorldMatrix(const core::matrix4& m)
			{
				m_world = m;
			}

			/**
			 * @brief Get the local-to-world matrix.
			 * @return Matrix4 reference.
			 */
			inline const core::matrix4& getWorld()
			{
				return m_world;
			}

			/**
			 * @brief Transform a local position to world space.
			 * @param pos Local position.
			 * @return World position.
			 */
			virtual core::vector3df getTransformPosition(const core::vector3df& pos);

			/**
			 * @brief Transform a local direction vector to world space.
			 * @param vec Local vector.
			 * @return World vector.
			 */
			virtual core::vector3df getTransformVector(const core::vector3df& vec);

			/**
			 * @brief Get the bounding box containing all active particles.
			 * @return AABB reference.
			 */
			inline const core::aabbox3df& getBBox()
			{
				return m_bbox;
			}

			/**
			 * @brief Get current particle count.
			 * @return Active particle count.
			 */
			inline u32 getNumParticles()
			{
				return m_particles.size();
			}

			/**
			 * @brief Get pointer to the internal particle array.
			 * @return Pointer to CParticle array.
			 */
			inline CParticle* getParticlePointer()
			{
				return m_particles.pointer();
			}

			/**
			 * @brief Add an emitter to this group.
			 * @param e Pointer to the emitter.
			 * @return Pointer to the added emitter.
			 */
			inline CEmitter* addEmitter(CEmitter* e)
			{
				m_emitters.push_back(e);
				return e;
			}

			/**
			 * @brief Get the list of emitters in this group.
			 * @return Reference to emitter vector.
			 */
			inline std::vector<CEmitter*>& getEmitters()
			{
				return m_emitters;
			}

			/**
			 * @brief Remove an emitter from this group.
			 * @param e Pointer to the emitter.
			 */
			void removeEmitter(CEmitter* e);

			/**
			 * @brief Add a callback for particle lifecycle events.
			 * @param cb Pointer to IParticleCallback.
			 */
			void addCallback(IParticleCallback* cb)
			{
				std::vector<IParticleCallback*>::iterator i = std::find(m_callback.begin(), m_callback.end(), cb);
				if (i == m_callback.end())
					m_callback.push_back(cb);
			}

			/**
			 * @brief Remove a lifecycle callback.
			 * @param cb Pointer to IParticleCallback.
			 */
			void removeCallback(IParticleCallback* cb)
			{
				std::vector<IParticleCallback*>::iterator i = std::find(m_callback.begin(), m_callback.end(), cb);
				if (i != m_callback.end())
					m_callback.erase(i);
			}

			/**
			 * @brief Get all lifecycle callbacks.
			 * @return Reference to callback vector.
			 */
			inline std::vector<IParticleCallback*>& getCallback()
			{
				return m_callback;
			}

			/**
			 * @brief Add a logic system to affect particles in this group.
			 * @param s Pointer to ISystem.
			 */
			inline void addSystem(ISystem* s)
			{
				m_systems.push_back(s);
			}

			/**
			 * @brief Get all systems assigned to this group.
			 * @return Reference to system vector.
			 */
			inline std::vector<ISystem*>& getSystems()
			{
				return m_systems;
			}

			/**
			 * @brief Remove a system from this group.
			 * @param s Pointer to the system.
			 */
			void removeSystem(ISystem* s);

			/**
			 * @brief Manually spawn a particle using a specific emitter.
			 * @param emiterID Index of the emitter.
			 * @param position Initial world position.
			 * @param subEmitterDirection Direction hint for sub-emitters.
			 * @return Index of the new particle, or -1 if failed.
			 */
			int addParticle(u32 emiterID, const core::vector3df& position, const core::vector3df& subEmitterDirection);

			/**
			 * @brief Manually spawn a particle with specific velocity.
			 * @param emiterID Index of the emitter.
			 * @param position Initial world position.
			 * @param velocity Initial world velocity.
			 * @return Index of the new particle, or -1 if failed.
			 */
			int addParticleVelocity(u32 emiterID, const core::vector3df& position, const core::vector3df& velocity);

			int addParticleByEmitter(CEmitter* emitter, const core::vector3df& position, const core::vector3df& subEmitterDirection);

			int addParticleVelocityByEmitter(CEmitter* emitter, const core::vector3df& position, const core::vector3df& velocity);

			/**
			 * @brief Set the renderer used to draw particles in this group.
			 * @param r Pointer to IRenderer.
			 * @return Pointer to the set renderer.
			 */
			IRenderer* setRenderer(IRenderer* r);

			/**
			 * @brief Get the active renderer.
			 * @return Pointer to IRenderer.
			 */
			inline IRenderer* getRenderer()
			{
				return m_renderer;
			}

			/**
			 * @brief Get internal GPU instancing data (for instanced renderers).
			 * @return Pointer to CParticleInstancing.
			 */
			CParticleInstancing* getIntancing()
			{
				return m_instancing;
			}

			/**
			 * @brief Get internal CPU mesh buffer (for non-instanced renderers).
			 * @return Pointer to CParticleCPUBuffer.
			 */
			CParticleCPUBuffer* getParticleBuffer()
			{
				return m_cpuBuffer;
			}

			/**
			 * @brief Get current particle count. Same as getNumParticles().
			 * @return Count of particles.
			 */
			inline u32 getCurrentParticleCount()
			{
				return m_particles.size();
			}

			/**
			 * @brief Create or retrieve an animation model for a parameter by name.
			 * @param attributeName Wstring identifier (e.g. L"Scale").
			 * @return Pointer to CModel.
			 */
			CModel* createModel(const std::wstring& attributeName);

			/**
			 * @brief Create or retrieve an animation model for a parameter by enum ID.
			 * @param param EParticleParams enum value.
			 * @return Pointer to CModel.
			 */
			CModel* createModel(EParticleParams param);

			/**
			 * @brief Get an existing model for a parameter.
			 * @param param EParticleParams enum value.
			 * @return Pointer to CModel, or NULL if not found.
			 */
			CModel* getModel(EParticleParams param);

			/**
			 * @brief Delete an animation model.
			 * @param param EParticleParams enum value.
			 */
			void deleteModel(EParticleParams param);

			/**
			 * @brief Get all models defined for this group.
			 * @return Reference to model vector.
			 */
			std::vector<CModel*>& getModels()
			{
				return m_models;
			}

			/**
			 * @brief Create a new interpolator for custom curve animation.
			 * @return Pointer to CInterpolator.
			 */
			CInterpolator* createInterpolator();

			/**
			 * @brief Delete an interpolator.
			 * @param interpolator Pointer to the interpolator.
			 */
			void deleteInterpolator(CInterpolator* interpolator);

			/**
			 * @brief Get all interpolators owned by this group.
			 * @return Reference to interpolator vector.
			 */
			std::vector<CInterpolator*>& getInterpolators()
			{
				return m_interpolators;
			}

			/**
			 * @brief Get the internal particle array.
			 * @return Reference to particle array.
			 */
			core::array<CParticle>& getParticles()
			{
				return m_particles;
			}

			/**
			 * @brief Get the frame index of the last update.
			 * @return Frame index.
			 */
			inline int getFrameUpdate()
			{
				return m_frameUpdate;
			}

			/**
			 * @brief Manually synchronize frame index.
			 * @param frame Current frame index.
			 */
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