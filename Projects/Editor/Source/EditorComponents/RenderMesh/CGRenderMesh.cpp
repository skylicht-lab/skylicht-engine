/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CGRenderMesh.h"

#include "Utils/CActivator.h"
#include "Components/CDependentComponent.h"

#include "GameObject/CGameObject.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGRenderMesh);

		DEPENDENT_COMPONENT(CRenderMesh, CGRenderMesh);

		CGRenderMesh::CGRenderMesh() :
			m_renderMesh(NULL)
		{

		}

		CGRenderMesh::~CGRenderMesh()
		{

		}

		void CGRenderMesh::initComponent()
		{
			m_renderMesh = m_gameObject->getComponent<CRenderMesh>();

			updateSelectBBox();
		}

		void CGRenderMesh::updateComponent()
		{
			updateSelectBBox();
		}

		void CGRenderMesh::updateSelectBBox()
		{
			CEntityManager* entityMgr = m_gameObject->getEntityManager();

			std::vector<CRenderMeshData*>& renderers = m_renderMesh->getRenderers();
			for (size_t i = 0, n = renderers.size(); i < n; i++)
			{
				CEntity* entity = renderers[i]->Entity;
				CSelectObjectData* selectObjectData = GET_ENTITY_DATA(entity, CSelectObjectData);
				if (selectObjectData == NULL)
					selectObjectData = entity->addData<CSelectObjectData>();

				// select bbox data
				selectObjectData->GameObject = m_gameObject;
				selectObjectData->BBox = renderers[i]->getMesh()->getBoundingBox();
			}
		}
	}
}