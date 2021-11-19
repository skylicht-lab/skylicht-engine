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

namespace Skylicht
{
	namespace Editor
	{
		CSelecting::CSelecting()
		{

		}

		CSelecting::~CSelecting()
		{

		}

		void CSelecting::end()
		{

		}

		bool CSelecting::OnEvent(const SEvent& event)
		{
			CHandles* handle = CHandles::getInstance();
			if (handle->isHoverOnAxisOrPlane())
				return false;

			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				int mouseX = event.MouseInput.X;
				int mouseY = event.MouseInput.Y;

				if (event.MouseInput.Event == EMIE_MOUSE_MOVED)
				{

				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{

				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					CSceneController* sceneController = CSceneController::getInstance();
					CSelectObjectSystem* selectSystem = sceneController->getSelectObjectSystem();

					core::line3df ray = CProjective::getViewRay(selectSystem->getCamera(), (float)mouseX, (float)mouseY, selectSystem->getViewportWidth(), selectSystem->getViewportHeight());

					float maxDistanceSQ = 500.0f * 500.0f;
					CGameObject* object = getObjectWithRay(ray, maxDistanceSQ);
					if (object != NULL)
					{
						CSelection* selection = CSelection::getInstance();

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
						// clear selection
						CSelection* selection = CSelection::getInstance();
						sceneController->deselectAllOnHierachy();
						selection->clear();
					}
				}

				return true;
			}

			return false;
		}

		CGameObject* CSelecting::getObjectWithRay(const core::line3d<f32>& ray, f32& outBestDistanceSquared)
		{
			CSelectObjectSystem* selectSystem = CSceneController::getInstance()->getSelectObjectSystem();
			core::array<CSelectObjectData*>& selectObjectData = selectSystem->getCulledCollision();

			core::vector3df point;
			core::vector3df vec = ray.getVector().normalize();
			const f32 lineLength = ray.getLengthSQ();

			CGameObject* result = NULL;

			for (u32 i = 0, n = selectObjectData.size(); i < n; i++)
			{
				CSelectObjectData* data = selectObjectData[i];

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
								result = data->GameObject;
							}
						}
					}
				}
			}

			return result;
		}
	}
}