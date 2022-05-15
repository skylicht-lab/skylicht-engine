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

#include "pch.h"
#include "CParticleBufferData.h"
#include "CParticleRenderer.h"
#include "CParticleComponent.h"

#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"

#include "Culling/CCullingData.h"
#include "Culling/CCullingBBoxData.h"
#include "Transform/CWorldInverseTransformData.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleComponent::CParticleComponent() :
			m_data(NULL)
		{

		}

		CParticleComponent::~CParticleComponent()
		{
			if (m_gameObject)
				m_gameObject->getEntity()->removeData<CParticleBufferData>();
		}

		void CParticleComponent::initComponent()
		{
			CEntity* entity = m_gameObject->getEntity();

			// add particle buffer data
			m_data = entity->addData<CParticleBufferData>();

			// add culling
			entity->addData<CCullingData>();
			entity->addData<CCullingBBoxData>();

			// add renderer system
			m_gameObject->getEntityManager()->addRenderSystem<CParticleRenderer>();
		}

		void CParticleComponent::updateComponent()
		{

		}

		void CParticleComponent::setGroupTransform(const core::matrix4& world)
		{
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				m_data->Groups[i]->setWorldMatrix(world);
			}
		}

		CGroup* CParticleComponent::createParticleGroup()
		{
			return m_data->createGroup();
		}

		CSubGroup* CParticleComponent::createParticleSubGroup(CGroup* group)
		{
			return m_data->createSubGroup(group);
		}

		void CParticleComponent::removeParticleGroup(CGroup* group)
		{
			m_data->removeGroup(group);
		}

		void CParticleComponent::Play()
		{
			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				std::vector<CEmitter*>& emitter = groups[i]->getEmitters();
				for (CEmitter* e : emitter)
				{
					e->resetTank();
				}
			}
		}

		void CParticleComponent::Stop()
		{
			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				std::vector<CEmitter*>& emitter = groups[i]->getEmitters();
				for (CEmitter* e : emitter)
				{
					e->stop();
				}
			}
		}

		bool CParticleComponent::IsPlaying()
		{
			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				if (groups[i]->getNumParticles() > 0)
					return true;
			}

			return false;
		}

		u32 CParticleComponent::getTotalParticle()
		{
			u32 n = 0;

			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
				n += groups[i]->getNumParticles();

			return n;
		}
	}
}