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

namespace Skylicht
{
	namespace Particle
	{
		struct SLaunchParticle
		{
			CEmitter *Emitter;
			u32 Number;
		};

		class CGroup
		{
		protected:
			core::array<CParticle> m_particles;
			core::array<SLaunchParticle> m_launch;
			std::vector<CEmitter*> m_emitters;
			std::vector<ISystem*> m_systems;

			CZone* m_zone;
			ISystem *m_defaultSystem;
			IRenderer* m_renderer;

		public:
			core::vector3df Gravity;
			float Friction;
			float LifeMin;
			float LifeMax;

		public:
			CGroup();

			virtual ~CGroup();

			void update();

			CZone* setZone(CZone *z)
			{
				m_zone = z;
				return m_zone;
			}

			inline CZone* getZone()
			{
				return m_zone;
			}

			CEmitter* addEmitter(CEmitter *e)
			{
				m_emitters.push_back(e);
				return e;
			}

			std::vector<CEmitter*>& getEmitters()
			{
				return m_emitters;
			}

			void removeEmitter(CEmitter *e)
			{
				std::vector<CEmitter*>::iterator i = std::find(m_emitters.begin(), m_emitters.end(), e);
				if (i != m_emitters.end())
					m_emitters.erase(i);
			}

			void addSystem(ISystem *s)
			{
				m_systems.push_back(s);
			}

			std::vector<ISystem*>& getSystems()
			{
				return m_systems;
			}

			void removeSystem(ISystem *s)
			{
				std::vector<ISystem*>::iterator i = std::find(m_systems.begin(), m_systems.end(), s);
				if (i != m_systems.end())
					m_systems.erase(i);
			}

		protected:
			bool launchParticle(CParticle& p, SLaunchParticle& launch);

			CParticle* create(u32 num);

			void remove(u32 i);
		};
	}
}