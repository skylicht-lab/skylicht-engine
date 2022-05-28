/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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

#pragma once

#include "pch.h"
#include "CGPrimitives.h"
#include "Transform/CWorldInverseTransformData.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGPrimitives);

		DEPENDENT_COMPONENT(CCube, CGPrimitives);

		DEPENDENT_COMPONENT(CSphere, CGPrimitives);

		CGPrimitives::CGPrimitives() :
			m_primitive(NULL)
		{

		}

		CGPrimitives::~CGPrimitives()
		{

		}

		void CGPrimitives::initComponent()
		{
			m_primitive = m_gameObject->getComponent<CPrimitive>();
		}

		void CGPrimitives::updateComponent()
		{
			updateSelectBBox();
		}

		void CGPrimitives::updateSelectBBox()
		{
			CEntityManager* entityMgr = m_gameObject->getEntityManager();

			std::vector<CEntity*>& entities = m_primitive->getEntities();
			for (size_t i = 0, n = entities.size(); i < n; i++)
			{
				CEntity* entity = entities[i];

				CSelectObjectData* selectObjectData = (CSelectObjectData*)entity->getDataByIndex(CSelectObjectData::DataTypeIndex);
				if (selectObjectData == NULL)
					selectObjectData = entity->addData<CSelectObjectData>();

				CWorldInverseTransformData* worldInv = (CWorldInverseTransformData*)entity->getDataByIndex(CWorldInverseTransformData::DataTypeIndex);
				if (worldInv == NULL)
					worldInv = entity->addData<CWorldInverseTransformData>();

				// select bbox data
				selectObjectData->GameObject = m_gameObject;
				selectObjectData->Entity = entity;

				selectObjectData->BBox.MinEdge = core::vector3df(-0.5f, -0.5f, -0.5f);
				selectObjectData->BBox.MaxEdge = core::vector3df(0.5f, 0.5f, 0.5f);
			}
		}
	}
}