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
#include "CPrimitiveRendererInstancing.h"
#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "Transform/CWorldTransformData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CPrimitiveRendererInstancing::CPrimitiveRendererInstancing()
	{

	}

	CPrimitiveRendererInstancing::~CPrimitiveRendererInstancing()
	{

	}

	void CPrimitiveRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		for (int i = 0; i < CPrimiviteData::Count; i++)
		{
			m_primitives[i].set_used(0);
			m_transforms[i].set_used(0);
		}
	}

	void CPrimitiveRendererInstancing::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CPrimiviteData* p = (CPrimiviteData*)entity->getDataByIndex(CPrimiviteData::DataTypeIndex);
		if (p != NULL)
		{
			CVisibleData* visible = (CVisibleData*)entity->getDataByIndex(CVisibleData::DataTypeIndex);
			if (visible->Visible &&
				p->Instancing &&
				p->Material &&
				p->Material->getShader() &&
				p->Material->getShader()->getInstancingShader())
			{
				m_primitives[p->Type].push_back(p);
			}
		}
	}

	void CPrimitiveRendererInstancing::init(CEntityManager* entityManager)
	{

	}

	extern int cmpPrimitiveFunc(const void* a, const void* b);

	void CPrimitiveRendererInstancing::update(CEntityManager* entityManager)
	{
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_mesh[type] == NULL)
				continue;

			core::array<CPrimiviteData*>& primitives = m_primitives[type];
			u32 count = primitives.size();

			// need sort by material
			qsort(primitives.pointer(), count, sizeof(CPrimiviteData*), cmpPrimitiveFunc);

			// get world transform			
			for (u32 i = 0; i < count; i++)
			{
				CEntity* entity = entityManager->getEntity(primitives[i]->EntityIndex);

				CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
				m_transforms[type].push_back(transform->World);
			}
		}
	}

	void CPrimitiveRendererInstancing::render(CEntityManager* entityManager)
	{
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_mesh[type] == NULL)
				continue;

		}
	}
}