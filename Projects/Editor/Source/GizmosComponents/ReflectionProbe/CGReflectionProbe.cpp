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
#include "CGReflectionProbe.h"
#include "Transform/CWorldInverseTransformData.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGReflectionProbe);

		DEPENDENT_COMPONENT(CReflectionProbe, CGReflectionProbe);

		CGReflectionProbe::CGReflectionProbe() :
			m_reflectionProbe(NULL)
		{

		}

		CGReflectionProbe::~CGReflectionProbe()
		{

		}

		void CGReflectionProbe::initComponent()
		{
			m_reflectionProbe = m_gameObject->getComponent<CReflectionProbe>();
		}

		void CGReflectionProbe::updateComponent()
		{
			updateSelectBBox();
		}

		void CGReflectionProbe::updateSelectBBox()
		{
			CEntity* entity = m_gameObject->getEntity();

			CSelectObjectData* selectObjectData = GET_ENTITY_DATA(entity, CSelectObjectData);
			if (selectObjectData == NULL)
				selectObjectData = entity->addData<CSelectObjectData>();

			CWorldInverseTransformData* worldInv = GET_ENTITY_DATA(entity, CWorldInverseTransformData);
			if (worldInv == NULL)
				worldInv = entity->addData<CWorldInverseTransformData>();

			// select bbox data
			selectObjectData->GameObject = m_gameObject;
			selectObjectData->BBox.MinEdge = core::vector3df(-0.3f, -0.3f, -0.3f);
			selectObjectData->BBox.MaxEdge = core::vector3df(0.3f, 0.3f, 0.3f);
		}
	}
}