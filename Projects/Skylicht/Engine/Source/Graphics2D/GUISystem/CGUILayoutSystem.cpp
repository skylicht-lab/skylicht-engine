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
#include "CGUILayoutSystem.h"

namespace Skylicht
{
	CGUILayoutSystem::CGUILayoutSystem()
	{

	}

	CGUILayoutSystem::~CGUILayoutSystem()
	{

	}

	void CGUILayoutSystem::beginQuery(CEntityManager* entityManager)
	{

	}

	void CGUILayoutSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
	{
		for (int i = 0; i < count; i++)
		{
			CEntity* e = entities[i];

			CWorldTransformData* w = GET_ENTITY_DATA(e, CWorldTransformData);
			CGUITransformData* t = GET_ENTITY_DATA(e, CGUITransformData);

			m_worldTransforms.push(w);
			m_guiTransform.push(t);
			m_guiAlign.push(GET_ENTITY_DATA(e, CGUIAlignData));
		}
	}

	void CGUILayoutSystem::init(CEntityManager* entityManager)
	{

	}

	void CGUILayoutSystem::update(CEntityManager* entityManager)
	{
		updateTransform();
	}

	void CGUILayoutSystem::updateTransform()
	{
		int numEntity = m_worldTransforms.count();

		CWorldTransformData** transforms = m_worldTransforms.pointer();
		CGUITransformData** guiTransforms = m_guiTransform.pointer();

		for (int i = 0; i < numEntity; i++)
		{
			CWorldTransformData* w = transforms[i];
			CGUITransformData* t = guiTransforms[i];

			if (w->Parent != NULL && t->isChanged())
			{
				t->setChanged(false);
			}
		}
	}
}