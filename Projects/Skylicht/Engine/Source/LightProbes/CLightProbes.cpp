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
#include "Serializable/CAttributeSerializable.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CLightProbes);

	CATEGORY_COMPONENT(CLightProbes, "Light Probes", "Indirect Lighting");

	CLightProbes::CLightProbes()
	{
		m_attributes = getIrrlichtDevice()->getFileSystem()->createEmptyAttributes();
	}

	CLightProbes::~CLightProbes()
	{
		clearAll();
		m_attributes->drop();
	}

	void CLightProbes::initComponent()
	{
		m_gameObject->getEntityManager()->addRenderSystem<CLightProbeRender>();

		// default 1 lightprobe
		CEntity* defaultLight = addLightProbe();

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
		m_attributes->clear();

		int numProbes = (int)m_entities.size();
		m_attributes->addInt("numprobes", numProbes);

		for (int i = 0; i < numProbes; i++)
		{
			CWorldTransformData* world = (CWorldTransformData*)m_entities[i]->getDataByIndex(CWorldTransformData::DataTypeIndex);
			CLightProbeData* light = (CLightProbeData*)m_entities[i]->getDataByIndex(CLightProbeData::DataTypeIndex);

			m_attributes->addString("entityID", m_entities[i]->getID().c_str());
			m_attributes->addMatrix("transform", world->Relative);
			for (int j = 0; j < 9; j++)
				m_attributes->addVector3d("sh", light->SH[j]);
		}

		CObjectSerializable* object = CComponentSystem::createSerializable();

		CAttributeSerializable* attributeObject = new CAttributeSerializable("ProbeData");
		attributeObject->deserialize(m_attributes);
		object->addProperty(attributeObject);
		object->addAutoRelease(attributeObject);

		return object;
	}

	void CLightProbes::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		CAttributeSerializable* attributeObject = object->getProperty<CAttributeSerializable>("ProbeData");
		if (attributeObject == NULL)
			return;

		attributeObject->serialize(m_attributes);
		clearAll();

		int id = 1; // first is numprobes

		int numProbe = m_attributes->getAttributeAsInt("numprobes");
		for (int i = 0; i < numProbe; i++)
		{
			CEntity* entity = addLightProbe();

			CWorldTransformData* world = (CWorldTransformData*)m_entities[i]->getDataByIndex(CWorldTransformData::DataTypeIndex);
			CLightProbeData* light = (CLightProbeData*)m_entities[i]->getDataByIndex(CLightProbeData::DataTypeIndex);

			core::stringc entityID = m_attributes->getAttributeAsString(id++);
			m_entities[i]->setID(entityID.c_str());

			world->Relative = m_attributes->getAttributeAsMatrix(id++);
			for (int j = 0; j < 9; j++)
			{
				light->SH[j] = m_attributes->getAttributeAsVector3d(id++);
			}
		}
	}

	void CLightProbes::clearAll()
	{
		for (CEntity* entity : m_probes)
			removeEntity(entity);
		m_probes.clear();
	}

	CEntity* CLightProbes::addLightProbe()
	{
		CEntity* entity = createEntity();

		entity->addData<CLightProbeData>();

		m_probes.push_back(entity);
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