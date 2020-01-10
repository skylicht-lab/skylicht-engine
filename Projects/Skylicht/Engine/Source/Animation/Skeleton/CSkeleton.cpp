/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CSkeleton.h"
#include "CAnimationTransformData.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	CSkeleton::CSkeleton(int id) :
		m_id(id),
		m_enable(true)
	{

	}

	CSkeleton::~CSkeleton()
	{
		releaseAllEntities();
	}

	void CSkeleton::initSkeleton(core::array<CEntity*>& entities)
	{
		// release old entities
		releaseAllEntities();

		std::map<int, int> entityIDMap;

		// we clone skeleton entities struct from render mesh entities
		CEntity** worldEntities = entities.pointer();

		for (u32 i = 0, n = entities.size(); i < n; i++)
		{
			CEntity* worldEntity = worldEntities[i];
			CWorldTransformData *worldTransform = worldEntity->getData<CWorldTransformData>();

			CEntity* newEntity = m_entites.createEntity();

			// map index
			entityIDMap[worldEntity->getIndex()] = newEntity->getIndex();

			// add animation transform data
			CAnimationTransformData *animationData = newEntity->addData<CAnimationTransformData>();
			animationData->Name = worldTransform->Name;
			animationData->WorldTransform = worldTransform;

			// find parent
			if (entityIDMap.find(worldTransform->ParentIndex) != entityIDMap.end())
			{
				animationData->ParentID = entityIDMap[worldTransform->ParentIndex];

				CAnimationTransformData *parentTransform = m_entites.getEntity(animationData->ParentID)->getData<CAnimationTransformData>();
				animationData->Depth = parentTransform->Depth + 1;
			}
			else
			{
				animationData->ParentID = -1;
				animationData->Depth = 0;
			}

			// default relative matrix
			animationData->DefaultRelativeMatrix = worldTransform->Relative;

			// default pos, scale, rot
			animationData->DefaultPosition = worldTransform->Relative.getTranslation();
			animationData->DefaultScale = worldTransform->Relative.getScale();
			animationData->DefaultRotation = core::quaternion(worldTransform->Relative);

			// default anim pos, scale, rot
			animationData->AnimPosition = animationData->DefaultPosition;
			animationData->AnimScale = animationData->DefaultScale;
			animationData->AnimRotation = animationData->DefaultRotation;
		}
	}

	void CSkeleton::releaseAllEntities()
	{
		m_entites.releaseAllEntities();
	}

	void CSkeleton::update()
	{

	}
}