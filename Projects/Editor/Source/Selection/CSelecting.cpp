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
#include "CSelecting.h"
#include "CSelection.h"
#include "Handles/CHandles.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Editor/SpaceController/CPropertyController.h"
#include "Projective/CProjective.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CSelecting);

		CSelecting::CSelecting() :
			m_altPressed(false),
			m_leftMousePressed(false),
			m_allowDragSelect(true)
		{

		}

		CSelecting::~CSelecting()
		{

		}

		void CSelecting::end()
		{
			m_altPressed = false;
			m_leftMousePressed = false;
		}

		bool CSelecting::OnEvent(const SEvent& event)
		{
			CHandles* handle = CHandles::getInstance();
			if (handle->isEnable())
			{
				if (handle->isHoverOnAxisOrPlane() || handle->isUsing())
					return false;
			}

			if (event.EventType == EET_KEY_INPUT_EVENT)
			{
				if (event.KeyInput.Key == irr::KEY_MENU ||
					event.KeyInput.Key == irr::KEY_LMENU ||
					event.KeyInput.Key == irr::KEY_RMENU)
				{
					m_altPressed = event.KeyInput.PressedDown;
					m_allowDragSelect = !m_altPressed;
				}
			}
			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				int mouseX = event.MouseInput.X;
				int mouseY = event.MouseInput.Y;

				m_mousePosition.set(mouseX, mouseY);
				if (event.MouseInput.Event == EMIE_MOUSE_MOVED)
				{

				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{
					m_leftMousePressed = true;
					m_mouseBegin = m_mousePosition;
				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					CSceneHistory* history = CSceneController::getInstance()->getHistory();
					history->enableAddSelectHistory(false);

					m_current.clear();

					if (!m_altPressed)
					{
						if (isDragSelect())
						{
							doMultiSelect();
						}
						else
						{
							doSingleSelect();
						}
					}

					// add select history
					history->enableAddSelectHistory(true);

					for (CGameObject* obj : m_current)
						history->beginSaveHistory(obj);

					history->addSelectHistory();

					m_leftMousePressed = false;
					m_allowDragSelect = true;
				}

				return true;
			}

			return false;
		}

		void CSelecting::doSingleSelect()
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CSelectObjectSystem* selectSystem = sceneController->getSelectObjectSystem();

			core::line3df ray = CProjective::getViewRay(selectSystem->getCamera(), (float)m_mousePosition.X, (float)m_mousePosition.Y, selectSystem->getViewportWidth(), selectSystem->getViewportHeight());

			float maxDistanceSQ = 500.0f * 500.0f;

			CGameObject* object = NULL;
			CEntity* entity = NULL;
			getObjectWithRay(ray, maxDistanceSQ, object, entity);

			CSelection* selection = CSelection::getInstance();

			if (object || entity)
			{
				doSelect(object, entity);
			}
			else
			{
				if (!GUI::CInput::getInput()->isKeyDown(GUI::EKey::KEY_CONTROL))
				{
					// clear selection
					sceneController->deselectAllOnHierachy();
					selection->clear();
				}
			}
		}

		void CSelecting::doSelect(CGameObject* object, CEntity* entity)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CSelection* selection = CSelection::getInstance();

			if (entity && object)
			{
				// force to select parent prefab
				std::string prefabId = object->getTemplateID();
				if (!prefabId.empty())
				{
					entity = NULL;
				}
			}

			if (entity != NULL)
			{
				if (!object->isLock())
				{
					if (!GUI::CInput::getInput()->isKeyDown(GUI::EKey::KEY_CONTROL))
					{
						// need clear current selection
						if (selection->getSelected(entity) == NULL)
						{
							sceneController->deselectAllOnHierachy();
							selection->clear();
						}
						sceneController->selectOnHierachy(entity);
					}
					else
					{
						// hold control && re-pick this entity
						if (selection->getSelected(entity) != NULL)
						{
							sceneController->deselectOnHierachy(entity);
						}
						else
						{
							sceneController->selectOnHierachy(entity);
						}
					}
				}

				if (object)
					m_current.push_back(object);
			}
			else if (object != NULL)
			{
				// try to select the parent prefab
				object = object->getParentTemplate();
				if (!object->isLock())
				{
					if (!GUI::CInput::getInput()->isKeyDown(GUI::EKey::KEY_CONTROL))
					{
						// need clear current selection
						if (selection->getSelected(object) == NULL)
						{
							sceneController->deselectAllOnHierachy();
							selection->clear();
						}
						sceneController->selectOnHierachy(object);
					}
					else
					{
						// hold control && re-pick this object
						if (selection->getSelected(object) != NULL)
						{
							sceneController->deselectOnHierachy(object);
						}
						else
						{
							sceneController->selectOnHierachy(object);
						}
					}
				}

				if (object && selection->getSelected(object))
					m_current.push_back(object);
			}
		}

		void CSelecting::addSelect(CGameObject* object, CEntity* entity)
		{
			CSceneController* sceneController = CSceneController::getInstance();

			if (entity && object)
			{
				// force to select parent prefab
				std::string prefabId = object->getTemplateID();
				if (!prefabId.empty())
				{
					entity = NULL;
				}
			}

			if (entity != NULL)
			{
				sceneController->selectOnHierachy(entity);
			}
			else if (object != NULL)
			{
				// try to select the parent prefab
				object = object->getParentTemplate();
				sceneController->selectOnHierachy(object);
			}

			// apply to save history
			if (object)
				m_current.push_back(object);
		}

		void CSelecting::doMultiSelect()
		{
			core::rectf selectRect;
			selectRect.UpperLeftCorner.X = (float)core::min_(m_mouseBegin.X, m_mousePosition.X);
			selectRect.UpperLeftCorner.Y = (float)core::min_(m_mouseBegin.Y, m_mousePosition.Y);
			selectRect.LowerRightCorner.X = (float)core::max_(m_mouseBegin.X, m_mousePosition.X);
			selectRect.LowerRightCorner.Y = (float)core::max_(m_mouseBegin.Y, m_mousePosition.Y);

			std::vector<CGameObject*> objects;
			std::vector<CEntity*> entites;

			getObjectInRect(selectRect, objects, entites);

			CSelection* selection = CSelection::getInstance();
			CSceneController* sceneController = CSceneController::getInstance();

			if (!GUI::CInput::getInput()->isKeyDown(GUI::EKey::KEY_CONTROL))
			{
				// clear selection
				sceneController->deselectAllOnHierachy();
				selection->clear();
			}

			int numObject = (int)objects.size();
			for (int i = 0; i < numObject; i++)
			{
				addSelect(objects[i], entites[i]);
			}
		}

		void CSelecting::getObjectWithRay(const core::line3d<f32>& ray, f32& outBestDistanceSquared, CGameObject*& object, CEntity*& entity)
		{
			object = NULL;
			entity = NULL;

			CSelectObjectSystem* selectSystem = CSceneController::getInstance()->getSelectObjectSystem();
			core::array<CSelectObjectData*>& selectObjectData = selectSystem->getCulledCollision();

			core::vector3df point;
			core::vector3df vec = ray.getVector().normalize();
			const f32 lineLength = ray.getLengthSQ();

			for (u32 i = 0, n = selectObjectData.size(); i < n; i++)
			{
				CSelectObjectData* data = selectObjectData[i];

				// skip lock object
				if (data->GameObject && data->GameObject->isLock())
					continue;

				// if ray hit this object
				if (data->TransformBBox.intersectsWithLine(ray))
				{
					// pick nearest object
					core::vector3df edges[8];
					data->TransformBBox.getEdges(edges);

					m_triangles[0].set(edges[3], edges[0], edges[2]);
					m_triangles[1].set(edges[3], edges[1], edges[0]);

					m_triangles[2].set(edges[3], edges[2], edges[7]);
					m_triangles[3].set(edges[7], edges[2], edges[6]);

					m_triangles[4].set(edges[7], edges[6], edges[4]);
					m_triangles[5].set(edges[5], edges[7], edges[4]);

					m_triangles[6].set(edges[5], edges[4], edges[0]);
					m_triangles[7].set(edges[5], edges[0], edges[1]);

					m_triangles[8].set(edges[1], edges[3], edges[7]);
					m_triangles[9].set(edges[1], edges[7], edges[5]);

					m_triangles[10].set(edges[0], edges[6], edges[2]);
					m_triangles[11].set(edges[0], edges[4], edges[6]);

					for (int i = 0; i < 12; i++)
					{
						const core::triangle3df& tris = m_triangles[i];
						if (tris.getIntersectionWithLine(ray.start, vec, point))
						{
							const f32 dis1 = point.getDistanceFromSQ(ray.start);
							const f32 dis2 = point.getDistanceFromSQ(ray.end);

							if (dis1 < lineLength && dis2 < lineLength && dis1 < outBestDistanceSquared)
							{
								outBestDistanceSquared = dis1;
								object = data->GameObject;
								entity = data->Entity;
							}
						}
					}
				}
			}
		}

		void CSelecting::getObjectInRect(const core::rectf& r, std::vector<CGameObject*>& objects, std::vector<CEntity*>& entities)
		{
			CSelectObjectSystem* selectSystem = CSceneController::getInstance()->getSelectObjectSystem();
			core::array<CSelectObjectData*>& selectObjectData = selectSystem->getCulledCollision();

			CCamera* camera = selectSystem->getCamera();
			core::dimension2df dim(
				(f32)selectSystem->getViewportWidth() * 0.5f,
				(f32)selectSystem->getViewportHeight() * 0.5f
			);

			core::matrix4 trans = camera->getProjectionMatrix();
			trans *= camera->getViewMatrix();

			core::vector2df v2d, center2d;

			for (u32 i = 0, n = selectObjectData.size(); i < n; i++)
			{
				CSelectObjectData* data = selectObjectData[i];

				// skip lock object
				if (data->GameObject && data->GameObject->isLock())
					continue;

				// convert 3d to 2d
				bool selected = false;
				
				core::vector3df edges[8];
				data->TransformBBox.getEdges(edges);
				
				if (project3Dto2D(data->TransformBBox.getCenter(), trans, dim, center2d))
				{
					if (r.isPointInside(center2d))
					{
						for (int i = 0; i < 8; i++)
						{
							if (project3Dto2D(edges[i], trans, dim, v2d))
							{
								if (r.isPointInside(v2d))
								{
									objects.push_back(data->GameObject);
									entities.push_back(data->Entity);
									break;
								}
							}
						}
					}
				}
			}
		}

		bool CSelecting::project3Dto2D(const core::vector3df& pos, core::matrix4& transform, core::dimension2df& dim, core::vector2df& out)
		{
			float transformedPos[4];

			transformedPos[0] = pos.X;
			transformedPos[1] = pos.Y;
			transformedPos[2] = pos.Z;
			transformedPos[3] = 1.0f;

			transform.multiplyWith1x4Matrix(transformedPos);
			f32 zDiv = transformedPos[3] == 0.0f ? 1.0f : core::reciprocal(transformedPos[3]);
			if (zDiv < 0)
				zDiv = -zDiv;

			out.X = dim.Width + dim.Width * transformedPos[0] * zDiv;
			out.Y = dim.Height - dim.Height * transformedPos[1] * zDiv;

			return transformedPos[3] >= 0;
		}

		bool CSelecting::isDragSelect(core::vector2di& from, core::vector2di& to)
		{
			if (!m_leftMousePressed || !m_allowDragSelect)
				return false;

			if (abs(m_mouseBegin.X - m_mousePosition.X) > 2 || abs(m_mouseBegin.Y - m_mousePosition.Y) > 2)
			{
				from.X = core::min_(m_mouseBegin.X, m_mousePosition.X);
				from.Y = core::min_(m_mouseBegin.Y, m_mousePosition.Y);
				to.X = core::max_(m_mouseBegin.X, m_mousePosition.X);
				to.Y = core::max_(m_mouseBegin.Y, m_mousePosition.Y);

				return true;
			}

			return false;
		}

		bool CSelecting::isDragSelect()
		{
			if (!m_leftMousePressed || !m_allowDragSelect)
				return false;

			if (abs(m_mouseBegin.X - m_mousePosition.X) > 2 || abs(m_mouseBegin.Y - m_mousePosition.Y) > 2)
			{
				return true;
			}

			return false;
		}
	}
}
