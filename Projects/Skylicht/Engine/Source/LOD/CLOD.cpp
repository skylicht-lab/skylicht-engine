#include "pch.h"
#include "CLOD.h"
#include "CLODData.h"
#include "RenderMesh/CRenderMesh.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CLOD::CLOD()
	{
		m_distance[0] = 100.0f;
		m_distance[1] = 500.0f;
		m_distance[2] = 1000.0f;
		m_distance[3] = 2000.0f;
	}

	CLOD::~CLOD()
	{

	}

	void CLOD::setLOD(float* lod, int count)
	{
		for (int i = 0; i < count && i < 4; i++)
		{
			m_distance[i] = lod[i];
		}
	}

	void CLOD::initComponent()
	{
		initLOD();
	}

	void CLOD::startComponent()
	{
		initLOD();
	}

	void CLOD::updateComponent()
	{

	}

	CObjectSerializable* CLOD::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();
		return object;
	}

	void CLOD::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);
	}

	void CLOD::initLOD()
	{
		CRenderMesh* renderMesh = m_gameObject->getComponent<CRenderMesh>();
		if (!renderMesh)
			return;

		core::array<CEntity*>& allEntities = renderMesh->getEntities();
		for (u32 i = 0, n = allEntities.size(); i < n; i++)
		{
			CEntity* e = allEntities[i];
			if (GET_ENTITY_DATA(e, CRenderMeshData))
			{
				CLODData* lodData = GET_ENTITY_DATA(e, CLODData);
				if (!lodData)
					lodData = e->addData<CLODData>();

				CWorldTransformData* t = GET_ENTITY_DATA(e, CWorldTransformData);

				char lodName[32];

				for (int i = 0; i <= 4; i++)
				{
					std::string name = CStringImp::toLower(t->Name);
					sprintf(lodName, "lod%d", i);

					// found LODx string in name
					if (CStringImp::find<const char>(name.c_str(), lodName) >= 0)
					{
						lodData->To = m_distance[i];
						lodData->From = 0.0f;
						if (i >= 1)
							lodData->From = m_distance[i - 1];

						lodData->From *= lodData->From;
						lodData->To *= lodData->To;

						break;
					}
				}
			}
		}
	}
}