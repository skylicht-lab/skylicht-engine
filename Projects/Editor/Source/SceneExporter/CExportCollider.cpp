/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CExportCollider.h"

#ifdef BUILD_SKYLICHT_PHYSIC
#include "Collider/CCollider.h"
#include "RigidBody/CRigidbody.h"
#endif

namespace Skylicht
{
	namespace Editor
	{
		CExportCollider::CExportCollider()
		{
			m_prefab = new CEntityPrefab();
		}

		CExportCollider::~CExportCollider()
		{
			delete m_prefab;
		}

		void CExportCollider::getBBox(CGameObject* object, core::aabbox3df& box, bool setBBox)
		{
#ifdef BUILD_SKYLICHT_PHYSIC
			if (!object->isVisible())
				return;

			Physics::CCollider* collider = object->getComponent<Physics::CCollider>();
			if (collider)
			{
				if (setBBox)
				{
					box = collider->getBBox();
					setBBox = false;
				}
				else
				{
					box.addInternalBox(collider->getBBox());
				}
			}

			CContainerObject* container = dynamic_cast<CContainerObject*>(object);
			if (container)
			{
				ArrayGameObject* childs = container->getChilds();
				for (CGameObject* go : *childs)
				{
					getBBox(go, box, false);
				}
			}
#endif
		}

		void CExportCollider::addGameObject(CGameObject* object, const core::aabbox3df& maxBbox)
		{
#ifdef BUILD_SKYLICHT_PHYSIC
			if (!object->isVisible())
				return;

			Physics::CCollider* collider = object->getComponent<Physics::CCollider>();
			Physics::CRigidbody* body = object->getComponent<Physics::CRigidbody>();

			if (collider && body)
			{
				CMesh* mesh = collider->generateMesh(maxBbox);
				if (mesh)
				{
					core::matrix4 world = body->getWorldTransform();

					std::string objName = object->getNameA();

					CWorldTransformData* transform = GET_ENTITY_DATA(object->getEntity(), CWorldTransformData);

					CEntity* entity = m_prefab->createEntity();

					CRenderMeshData* renderData = entity->addData<CRenderMeshData>();
					renderData->setMesh(mesh);

					std::string renderName = objName;
					renderName += "_";
					renderName += transform->Name;

					m_prefab->addTransformData(entity, NULL, world, renderName.c_str());

					CWorldTransformData* transformData = entity->getData<CWorldTransformData>();
					transformData->World = world;

					mesh->drop();
				}
			}

			CContainerObject* container = dynamic_cast<CContainerObject*>(object);
			if (container)
			{
				ArrayGameObject* childs = container->getChilds();
				for (CGameObject* go : *childs)
				{
					addGameObject(go, maxBbox);
				}
			}
#endif
		}
	}
}