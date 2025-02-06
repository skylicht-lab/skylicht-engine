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
#include "CExportRenderMesh.h"

namespace Skylicht
{
	namespace Editor
	{
		CExportRenderMesh::CExportRenderMesh()
		{
			m_prefab = new CEntityPrefab();
		}

		CExportRenderMesh::~CExportRenderMesh()
		{
			delete m_prefab;
		}

		void CExportRenderMesh::addGameObject(CGameObject* object)
		{
			if (!object->isVisible())
				return;

			CRenderMesh* renderMesh = object->getComponent<CRenderMesh>();
			if (renderMesh)
			{
				std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
				std::vector<CWorldTransformData*>& transforms = renderMesh->getRenderTransforms();

				std::string objName = object->getNameA();

				for (u32 i = 0, n = (u32)renderers.size(); i < n; i++)
				{
					CEntity* entity = m_prefab->createEntity();
					CRenderMeshData* renderData = entity->addData<CRenderMeshData>();
					renderData->setMesh(renderers[i]->getMesh());

					std::string renderName = objName;
					renderName += "_";
					renderName += transforms[i]->Name;

					m_prefab->addTransformData(entity, NULL, transforms[i]->World, renderName.c_str());

					CWorldTransformData* transformData = entity->getData<CWorldTransformData>();
					transformData->World = transforms[i]->World;
				}
			}

			CContainerObject* container = dynamic_cast<CContainerObject*>(object);
			if (container)
			{
				ArrayGameObject* childs = container->getChilds();
				for (CGameObject* go : *childs)
				{
					addGameObject(go);
				}
			}
		}
	}
}