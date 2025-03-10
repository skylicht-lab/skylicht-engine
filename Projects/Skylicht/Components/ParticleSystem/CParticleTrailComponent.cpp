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
#include "CParticleTrailComponent.h"
#include "CParticleTrailRenderer.h"
#include "CParticleComponent.h"

#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"

#include "Utils/CStringImp.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	namespace Particle
	{
		ACTIVATOR_REGISTER(CParticleTrailComponent);

		CATEGORY_COMPONENT(CParticleTrailComponent, "Particle Trail", "Particle");

		CParticleTrailComponent::CParticleTrailComponent() :
			m_data(NULL)
		{

		}

		CParticleTrailComponent::~CParticleTrailComponent()
		{

		}

		void CParticleTrailComponent::initComponent()
		{
			CEntity* entity = m_gameObject->getEntity();

			// add particle buffer data
			m_data = entity->addData<CParticleTrailData>();

			// add renderer system
			m_gameObject->getEntityManager()->addRenderSystem<CParticleTrailRenderer>();
		}

		void CParticleTrailComponent::updateComponent()
		{
			CWorldTransformData* transform = GET_ENTITY_DATA(m_gameObject->getEntity(), CWorldTransformData);

			core::array<CParticleTrail*>& trails = m_data->Trails;
			for (u32 i = 0; i < trails.size(); i++)
			{
				trails[i]->setWorld(transform->World);
			}
		}

		CParticleTrail* CParticleTrailComponent::addTrail(CGroup* group)
		{
			return m_data->addTrail(group);
		}

		CObjectSerializable* CParticleTrailComponent::createSerializable()
		{
			CObjectSerializable* object = CComponentSystem::createSerializable();

			CArrayTypeSerializable<CTrailSerializable>* arrayTrailData = new CArrayTypeSerializable<CTrailSerializable>("Trails");
			object->addProperty(arrayTrailData);
			object->autoRelease(arrayTrailData);

			core::array<CParticleTrail*>& trails = m_data->Trails;
			for (u32 i = 0; i < trails.size(); i++)
			{
				CParticleTrail* trail = trails[i];
				CTrailSerializable* trailData = (CTrailSerializable*)arrayTrailData->createElement();

				trailData->Name.set(CStringImp::convertUnicodeToUTF8(trail->getGroupName()));
				trailData->Width.set(trail->getWidth());
				trailData->Length.set(trail->getLength());
				trailData->SegmentLength.set(trail->getSegmentLength());
				trailData->DestroyWhenParticleDead.set(trail->isDestroyedWhenParticleDead());
				trailData->DeadAlphaReduction.set(trail->getDeadAlphaReduction());
				trailData->Texture.set(trail->getTexturePath());
				trailData->UseCustomMaterial.set(trail->useCustomMaterial());
				trailData->CustomMaterial.set(trail->getCustomMaterial());
			}

			return object;
		}

		void CParticleTrailComponent::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);

			CParticleComponent* particle = m_gameObject->getComponent<CParticleComponent>();
			if (particle == NULL)
				return;

			CArraySerializable* arrayTrailData = (CArraySerializable*)object->getProperty("Trails");
			if (arrayTrailData == NULL)
				return;

			int numTrails = arrayTrailData->getElementCount();

			m_data->removeAllTrail();

			for (int i = 0; i < numTrails; i++)
			{
				CTrailSerializable* trailData = (CTrailSerializable*)arrayTrailData->getElement(i);
				if (trailData == NULL)
					return;

				std::wstring name = CStringImp::convertUTF8ToUnicode(trailData->Name.getString());

				CGroup* group = particle->getData()->getGroupByName(name.c_str());
				if (group)
				{
					CParticleTrail* trail = m_data->addTrail(group);
					trail->setWidth(trailData->Width.get());
					trail->setLength(trailData->Length.get());
					trail->setSegmentLength(trailData->SegmentLength.get());
					trail->enableDestroyWhenParticleDead(trailData->DestroyWhenParticleDead.get());
					trail->setDeadAlphaReduction(trailData->DeadAlphaReduction.get());
					trail->setTexturePath(trailData->Texture.get().c_str());

					std::string materialPath = trailData->CustomMaterial.get();
					if (!materialPath.empty())
					{
						ArrayMaterial& loadMaterials = CMaterialManager::getInstance()->loadMaterial(materialPath.c_str(), true, std::vector<std::string>());
						if (loadMaterials.size() > 0)
						{
							trail->setCustomMaterial(loadMaterials[0]);
							trail->enableCustomMaterial(trailData->UseCustomMaterial.get());
						}
						else
						{
							trail->enableCustomMaterial(false);
						}
					}
				}
			}
		}
	}
}