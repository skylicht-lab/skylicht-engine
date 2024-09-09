/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CLightProbes.h"
#include "CLightProbeData.h"
#include "CLightProbeRender.h"
#include "CProbeSerializable.h"

#include "Entity/CEntityManager.h"
#include "GameObject/CGameObject.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CLightProbes);

	CATEGORY_COMPONENT(CLightProbes, "Light Probes", "Indirect Lighting");

	CLightProbes::CLightProbes()
	{
	}

	CLightProbes::~CLightProbes()
	{
	}

	void CLightProbes::initComponent()
	{
		m_gameObject->getEntityManager()->addRenderSystem<CLightProbeRender>();

		// default 1 lightprobe
		CEntity* defaultLight = addLightProbe(core::vector3df());

		// config default ambient color
		CLightProbeData* data = GET_ENTITY_DATA(defaultLight, CLightProbeData);
		data->SH[0].set(1.826f, 2.073f, 2.362f);
		data->SH[1].set(0.255f, 0.497f, 0.822f);
		data->SH[2].set(0.089f, 0.003f, -0.042f);
		data->SH[3].set(-0.049f, -0.059f, -0.07f);
		data->SH[4].set(0.015f, 0.013f, 0.007f);
		data->SH[5].set(0.161f, 0.064f, 0.015f);
		data->SH[6].set(0.154f, 0.102f, 0.01f);
		data->SH[7].set(0.0f, -0.0f, 0.0f);
		data->SH[8].set(0.151f, 0.126f, 0.005f);
	}

	void CLightProbes::updateComponent()
	{

	}

	CObjectSerializable* CLightProbes::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		CArraySerializable* probes = new CArraySerializable("Probes");
		object->addProperty(probes);
		object->autoRelease(probes);

		int numProbes = (int)m_entities.size();
		for (int i = 0; i < numProbes; i++)
		{
			CProbeSerializable* probeData = new CProbeSerializable(probes);
			probes->autoRelease(probeData);

			CWorldTransformData* world = GET_ENTITY_DATA(m_entities[i], CWorldTransformData);
			CLightProbeData* light = GET_ENTITY_DATA(m_entities[i], CLightProbeData);

			// save transform
			probeData->Transform.set(world->Relative);

			// save sh data
			for (int j = 0; j < 9; j++)
				probeData->SH[j]->set(light->SH[j]);
		}

		return object;
	}

	void CLightProbes::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		CArraySerializable* probes = (CArraySerializable*)object->getProperty("Probes");
		if (probes == NULL)
			return;

		int numProbes = probes->getElementCount();

		removeAllEntities();

		for (int i = 0; i < numProbes; i++)
		{
			CProbeSerializable* shData = (CProbeSerializable*)probes->getElement(i);
			if (shData == NULL)
				return;

			CEntity* entity = addLightProbe(core::vector3df());
			CWorldTransformData* world = GET_ENTITY_DATA(entity, CWorldTransformData);
			CLightProbeData* light = GET_ENTITY_DATA(entity, CLightProbeData);

			// set transform
			world->Relative = shData->Transform.get();

			// set sh data
			for (int j = 0; j < 9; j++)
				light->SH[j] = shData->SH[j]->get();
		}
	}

	CEntity* CLightProbes::spawn()
	{
		return addLightProbe(core::vector3df());
	}

	CEntity* CLightProbes::addLightProbe(const core::vector3df& position)
	{
		CEntity* entity = createEntity();
		entity->addData<CLightProbeData>();

		CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
		transform->Relative.setTranslation(position);

		return entity;
	}

	int CLightProbes::getPositions(std::vector<core::vector3df>& positions)
	{
		positions.clear();

		for (u32 i = 0, n = (u32)m_entities.size(); i < n; i++)
		{
			CWorldTransformData* data = GET_ENTITY_DATA(m_entities[i], CWorldTransformData);
			positions.push_back(data->World.getTranslation());
		}

		return (int)positions.size();
	}

	void CLightProbes::setSH(std::vector<core::vector3df>& sh)
	{
		int i = 0;
		for (u32 it = 0, n = m_entities.size(); it < n; it++)
		{
			CLightProbeData* data = GET_ENTITY_DATA(m_entities[it], CLightProbeData);
			data->NeedValidate = true;
			for (int j = 0; j < 9; j++)
				data->SH[j] = sh[i++];
		}
	}
}