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
#include "CSelectObjectSystem.h"

#include "Culling/CCullingData.h"
#include "Culling/CVisibleData.h"

#include "Entity/CEntityManager.h"

#include "Handles/CHandles.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CSelectObjectSystem::CSelectObjectSystem() :
			m_camera(NULL),
			m_skipUpdate(false)
		{

		}

		CSelectObjectSystem::~CSelectObjectSystem()
		{

		}

		void CSelectObjectSystem::beginQuery(CEntityManager* entityManager)
		{
			if (entityManager->getCamera() != m_camera)
			{
				m_skipUpdate = true;
				return;
			}

			m_results.set_used(0);
			m_collision.set_used(0);
			m_transform.set_used(0);

			m_skipUpdate = false;
		}

		void CSelectObjectSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{
			if (m_skipUpdate)
			{
				// just set transform changed
				for (int i = 0; i < numEntity; i++)
				{
					CEntity* entity = entities[i];

					CSelectObjectData* collisionData = GET_ENTITY_DATA(entity, CSelectObjectData);
					if (collisionData != NULL)
					{
						CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
						collisionData->TransformChanged |= transform->NeedValidate;
					}
				}
				return;
			}

			for (int i = 0; i < numEntity; i++)
			{
				CEntity* entity = entities[i];

				CSelectObjectData* collisionData = GET_ENTITY_DATA(entity, CSelectObjectData);
				if (collisionData != NULL)
				{
					CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
					collisionData->TransformChanged |= transform->NeedValidate;

					// check this entity is visible?
					CVisibleData* visibleData = GET_ENTITY_DATA(entity, CVisibleData);
					if (visibleData != NULL && !visibleData->Visible)
						continue;

					// check this entity is culled from camera?
					CCullingData* cullingData = GET_ENTITY_DATA(entity, CCullingData);
					if (cullingData != NULL && cullingData->CameraCulled)
						continue;

					m_collision.push_back(collisionData);
					m_transform.push_back(transform);
				}
			}
		}

		void CSelectObjectSystem::init(CEntityManager* entityManager)
		{

		}

		void CSelectObjectSystem::update(CEntityManager* entityManager)
		{
			if (m_skipUpdate)
				return;

			CSelectObjectData** collisions = m_collision.pointer();
			CWorldTransformData** transforms = m_transform.pointer();

			CCamera* camera = entityManager->getCamera();

			std::map<CGameObject*, core::aabbox3df>& selectedBox = m_gameObjBBox;
			selectedBox.clear();

			CSelection* selection = CSelection::getInstance();

			u32 numEntity = m_collision.size();
			for (u32 i = 0; i < numEntity; i++)
			{
				CWorldTransformData* transform = transforms[i];
				CSelectObjectData* collision = collisions[i];

				// 0. Update transform
				if (collision->TransformChanged)
				{
					collision->TransformBBox = collision->BBox;
					collision->TransformChanged = false;

					transform->World.transformBoxEx(collision->TransformBBox);
				}

				// 1. Detect by bounding box
				const SViewFrustum& frust = camera->getViewFrustum();
				bool visible = collision->TransformBBox.intersectsWithBox(frust.getBoundingBox());

				if (visible)
				{
					m_results.push_back(collision);

					// select on game object
					if (collision->DrawSelectionBox)
					{
						bool checkParentSelected = true;

						if (selection->getSelected(collision->GameObject))
						{
							checkParentSelected = false;
							if (selectedBox.find(collision->GameObject) == selectedBox.end())
								selectedBox[collision->GameObject] = collision->TransformBBox;
							else
								selectedBox[collision->GameObject].addInternalBox(collision->TransformBBox);
						}
						else if (collision->Entity)
						{
							// select on entity
							if (selection->getSelected(collision->Entity))
							{
								checkParentSelected = false;
								if (selectedBox.find(collision->GameObject) == selectedBox.end())
									selectedBox[collision->GameObject] = collision->TransformBBox;
								else
									selectedBox[collision->GameObject].addInternalBox(collision->TransformBBox);
							}
						}

						if (checkParentSelected && collision->GameObject)
						{
							// try search in parent
							CGameObject* parent = collision->GameObject->getParent();
							while (parent != NULL)
							{
								if (selection->getSelected(parent))
								{
									if (selectedBox.find(parent) == selectedBox.end())
										selectedBox[parent] = collision->TransformBBox;
									else
										selectedBox[parent].addInternalBox(collision->TransformBBox);
									break;
								}
								parent = parent->getParent();
							}
						}
					}

					// debug draw all box
					// CHandles::getInstance()->draw3DBox(collision->TransformBBox, SColor(200, 255, 255, 0));
				}
			}

			for (const auto& i : selectedBox)
			{
				// Draw bbox for selected object
				CHandles::getInstance()->draw3DBox(i.second, SColor(200, 0, 255, 0));
			}
		}

		bool CSelectObjectSystem::getTransformBBox(CGameObject* obj, core::aabbox3df& result)
		{
			auto i = m_gameObjBBox.find(obj);
			if (i == m_gameObjBBox.end())
			{
				core::vector3df size(1.0f, 1.0f, 1.0f);
				core::vector3df pos = obj->getPosition();
				result.MinEdge = pos - size;
				result.MaxEdge = pos + size;
				return false;
			}
			result = i->second;
			return true;
		}
	}
}
