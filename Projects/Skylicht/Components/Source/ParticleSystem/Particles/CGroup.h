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
#include "Systems/CParticleBufferSystem.h"

#include "CParticleInstancing.h"
#include "CModel.h"

namespace Skylicht
{
	namespace Particle
	{
		struct SLaunchParticle
		{
			CEmitter *Emitter;
			u32 Number;
		};

		class IParticleCallback
		{
		public:
			IParticleCallback()
			{

			}

			virtual ~IParticleCallback()
			{

			}

			virtual void OnParticleUpdate(CParticle *particles, int num, CGroup *group, float dt)
			{

			}

			virtual void OnParticleBorn(CParticle &p)
			{

			}

			virtual void OnParticleDead(CParticle &p, int index)
			{

			}

			virtual void OnSwapParticleData(CParticle &p1, CParticle &p2)
			{

			}
		};

		class CGroup
		{
		protected:
			core::array<CParticle> m_particles;
			core::array<SLaunchParticle> m_launch;

			std::vector<CEmitter*> m_emitters;
			std::vector<ISystem*> m_systems;
			std::vector<CModel*> m_models;
			std::vector<CInterpolator*> m_interpolators;

			CParticleSystem *m_particleSystem;
			CParticleBufferSystem *m_bufferSystem;

			IRenderer* m_renderer;
			CParticleInstancing *m_instancing;

			IParticleCallback *m_callback;

		public:
			core::vector3df Gravity;
			float Friction;
			float LifeMin;
			float LifeMax;

			core::vector3df OrientationNormal;
			core::vector3df OrientationUp;

		protected:
			core::matrix4 m_world;
			core::aabbox3df m_bbox;

		public:
			CGroup();

			virtual ~CGroup();

			void update(bool visible);

			inline void setWorldMatrix(const core::matrix4& m)
			{
				m_world = m;
			}

			inline const core::matrix4& getWorld()
			{
				return m_world;
			}

			core::vector3df getTransformPosition(const core::vector3df& pos);

			core::vector3df getTransformVector(const core::vector3df& vec);

			inline const core::aabbox3df& getBBox()
			{
				return m_bbox;
			}

			inline u32 getNumParticles()
			{
				return m_particles.size();
			}

			inline CEmitter* addEmitter(CEmitter *e)
			{
				m_emitters.push_back(e);
				return e;
			}

			inline std::vector<CEmitter*>& getEmitters()
			{
				return m_emitters;
			}

			void removeEmitter(CEmitter *e)
			{
				std::vector<CEmitter*>::iterator i = std::find(m_emitters.begin(), m_emitters.end(), e);
				if (i != m_emitters.end())
					m_emitters.erase(i);
			}

			inline void setCallback(IParticleCallback *cb)
			{
				m_callback = cb;
			}

			inline IParticleCallback* getCallback()
			{
				return m_callback;
			}

			inline void addSystem(ISystem *s)
			{
				m_systems.push_back(s);
			}

			inline std::vector<ISystem*>& getSystems()
			{
				return m_systems;
			}

			void removeSystem(ISystem *s)
			{
				std::vector<ISystem*>::iterator i = std::find(m_systems.begin(), m_systems.end(), s);
				if (i != m_systems.end())
					m_systems.erase(i);
			}

			void addParticle(const core::vector3df& position, CEmitter *emitter);

			IRenderer* setRenderer(IRenderer *r);

			inline IRenderer* getRenderer()
			{
				return m_renderer;
			}

			CParticleInstancing* getIntancing()
			{
				return m_instancing;
			}

			inline u32 getCurrentParticleCount()
			{
				return m_particles.size();
			}

			CModel* createModel(EParticleParams param);

			CModel* getModel(EParticleParams param);

			void deleteModel(EParticleParams param);

			std::vector<CModel*>& getModels()
			{
				return m_models;
			}

			CInterpolator* createInterpolator();

			void deleteInterpolator(CInterpolator* interpolator);

			std::vector<CInterpolator*>& getInterpolators()
			{
				return m_interpolators;
			}

		protected:
			bool launchParticle(CParticle& p, SLaunchParticle& launch);

			void initParticleModel(CParticle& p);

			inline void initParticleLifeTime(CParticle& p)
			{
				p.Age = 0.0f;
				p.Life = random(LifeMin, LifeMax);
				p.LifeTime = p.Life;
				p.HaveRotate = false;
			}

			CParticle* create(u32 num);

			void remove(u32 i);
		};
	}
}