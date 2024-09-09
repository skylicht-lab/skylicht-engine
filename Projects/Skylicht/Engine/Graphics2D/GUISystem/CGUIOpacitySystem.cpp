/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CGUIOpacitySystem.h"

namespace Skylicht
{
	CGUIOpacitySystem::CGUIOpacitySystem()
	{

	}

	CGUIOpacitySystem::~CGUIOpacitySystem()
	{

	}

	void CGUIOpacitySystem::beginQuery(CEntityManager* entityManager)
	{
		m_worldTransforms.reset();
		m_guiRenderData.reset();
	}

	void CGUIOpacitySystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
	{
		for (int i = 0; i < count; i++)
		{
			CEntity* e = entities[i];

			CWorldTransformData* w = GET_ENTITY_DATA(e, CWorldTransformData);
			CGUIRenderData* r = GET_ENTITY_DATA(e, CGUIRenderData);

			if (r->AffectOpacityInChild)
			{
				m_worldTransforms.push(w);
				m_guiRenderData.push(r);
			}
			else if (w->Parent)
			{
				CGUIRenderData* parentData = GET_ENTITY_DATA(w->Parent->Entity, CGUIRenderData);
				if (parentData->AffectOpacityInChild || parentData->m_applyParentOpacity)
				{
					m_worldTransforms.push(w);
					m_guiRenderData.push(r);

					r->m_applyParentOpacity = true;
				}
				else
				{
					r->m_applyParentOpacity = false;
				}
			}
			else
			{
				r->m_applyParentOpacity = false;
			}
		}
	}

	void CGUIOpacitySystem::init(CEntityManager* entityManager)
	{

	}

	void CGUIOpacitySystem::update(CEntityManager* entityManager)
	{
		int numEntity = m_guiRenderData.count();

		CWorldTransformData** transformData = m_worldTransforms.pointer();
		CGUIRenderData** renderData = m_guiRenderData.pointer();

		for (int i = 0; i < numEntity; i++)
		{
			CGUIRenderData* r = renderData[i];

			if (r->AffectOpacityInChild)
			{
				r->m_opacity = r->Color.getAlpha() / 255.0f;
			}
			else
			{
				CWorldTransformData* w = transformData[i];

				CGUIRenderData* parentData = GET_ENTITY_DATA(w->Parent->Entity, CGUIRenderData);
				r->m_opacity = parentData->m_opacity;
			}
		}
	}
}