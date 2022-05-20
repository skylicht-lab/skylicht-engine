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
		clearAll();
	}

	void CLightProbes::initComponent()
	{
		m_gameObject->getEntityManager()->addRenderSystem<CLightProbeRender>();

		// default 1 lightprobe
		CEntity* defaultLight = addLightProbe(core::vector3df());

		// config default ambient color
		CLightProbeData* data = (CLightProbeData*)defaultLight->getDataByIndex(CLightProbeData::DataTypeIndex);
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

		// num probes
		int numProbes = (int)m_entities.size();
		object->addAutoRelease(new CIntProperty(object, "numprobes", numProbes));

		// probes container
		CObjectSerializable* probes = new CObjectSerializable("probes");
		object->addProperty(probes);
		object->addAutoRelease(probes);

		// entity data
		for (int i = 0; i < numProbes; i++)
		{
			CObjectSerializable* shData = new CObjectSerializable("data");
			probes->addProperty(shData);
			probes->addAutoRelease(shData);

			CWorldTransformData* world = (CWorldTransformData*)m_entities[i]->getDataByIndex(CWorldTransformData::DataTypeIndex);
			CLightProbeData* light = (CLightProbeData*)m_entities[i]->getDataByIndex(CLightProbeData::DataTypeIndex);

			shData->addAutoRelease(new CStringProperty(shData, "entityID", m_entities[i]->getID().c_str()));
			shData->addAutoRelease(new CMatrixProperty(shData, "transform", world->Relative));

			for (int j = 0; j < 9; j++)
			{
				char name[64];
				sprintf(name, "sh%d", j);
				shData->addAutoRelease(new CVector3Property(shData, name, light->SH[j]));
			}
		}

		return object;
	}

	void CLightProbes::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		int numProbes = object->get<int>("numprobes", 0);

		CObjectSerializable* probes = (CObjectSerializable*)object->getProperty("probes");

		clearAll();

		if (probes == NULL)
			return;

		if (probes->getNumProperty() < numProbes)
			return;

		for (int i = 0; i < numProbes; i++)
		{
			CObjectSerializable* shData = (CObjectSerializable*)probes->getPropertyID(i);
			if (shData == NULL)
				return;

			CEntity* entity = addLightProbe(core::vector3df());

			CWorldTransformData* world = (CWorldTransformData*)m_entities[i]->getDataByIndex(CWorldTransformData::DataTypeIndex);
			CLightProbeData* light = (CLightProbeData*)m_entities[i]->getDataByIndex(CLightProbeData::DataTypeIndex);

			std::string entityID = shData->get<std::string>("entityID", std::string());
			m_entities[i]->setID(entityID.c_str());

			world->Relative = shData->get<core::matrix4>("transform", core::IdentityMatrix);
			for (int j = 0; j < 9; j++)
			{
				char name[64];
				sprintf(name, "sh%d", j);
				light->SH[j] = shData->get<core::vector3df>(name, core::vector3df());
			}
		}
	}

	void CLightProbes::clearAll()
	{
		removeAllEntities();
	}

	CEntity* CLightProbes::addLightProbe(const core::vector3df& position)
	{
		CEntity* entity = createEntity();
		entity->addData<CLightProbeData>();

		CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
		transform->Relative.setTranslation(position);

		return entity;
	}

	int CLightProbes::getPositions(std::vector<core::vector3df>& positions)
	{
		positions.clear();

		for (CEntity* entity : m_entities)
		{
			CWorldTransformData* data = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
			positions.push_back(data->World.getTranslation());
		}

		return (int)positions.size();
	}

	void CLightProbes::setSH(std::vector<core::vector3df>& sh)
	{
		int i = 0;
		for (CEntity* entity : m_entities)
		{
			CLightProbeData* data = (CLightProbeData*)entity->getDataByIndex(CLightProbeData::DataTypeIndex);
			for (int j = 0; j < 9; j++)
				data->SH[j] = sh[i++];
		}
	}
}