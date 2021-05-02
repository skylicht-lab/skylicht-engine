/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CViewpointRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		CViewpointRenderer::CViewpointRenderer()
		{
		}

		CViewpointRenderer::~CViewpointRenderer()
		{

		}

		void CViewpointRenderer::beginQuery()
		{
			m_viewpoints.set_used(0);
			m_transforms.set_used(0);
		}

		void CViewpointRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
		{
			CViewpointData* viewPointData = entity->getData<CViewpointData>();

			if (viewPointData != NULL)
			{
				CVisibleData* visible = entity->getData<CVisibleData>();
				CWorldTransformData* transformData = entity->getData<CWorldTransformData>();

				if (transformData != NULL && visible->Visible)
				{
					m_viewpoints.push_back(viewPointData);
					m_transforms.push_back(transformData);
				}
			}
		}

		void CViewpointRenderer::init(CEntityManager* entityManager)
		{

		}

		void CViewpointRenderer::update(CEntityManager* entityManager)
		{

		}

		void CViewpointRenderer::render(CEntityManager* entityManager)
		{
			IVideoDriver* driver = getVideoDriver();

			CViewpointData** viewpoints = m_viewpoints.pointer();
			CWorldTransformData** transforms = m_transforms.pointer();

			irr::core::matrix4 invModelView;
			{
				irr::core::matrix4 modelView(driver->getTransform(video::ETS_VIEW));
				modelView.getInversePrimitive(invModelView); // wont work for odd modelview matrices (but should happen in very special cases)
			}

			core::vector3df look(invModelView[8], invModelView[9], invModelView[10]);
			core::vector3df up(invModelView[4], invModelView[5], invModelView[6]);

			look.normalize();
			up.normalize();

			for (u32 i = 0, n = m_viewpoints.size(); i < n; i++)
			{
				driver->setTransform(video::ETS_WORLD, transforms[i]->World);
				IMeshBuffer* line = viewpoints[i]->LineBuffer;
				driver->setMaterial(line->getMaterial());
				driver->drawMeshBuffer(line);
			}

			for (u32 i = 0, n = m_viewpoints.size(); i < n; i++)
			{
				driver->setTransform(video::ETS_WORLD, transforms[i]->World);
				viewpoints[i]->updateBillboard(look, up);				

				IMeshBuffer* buffer = viewpoints[i]->Buffer;
				driver->setMaterial(buffer->getMaterial());
				driver->drawMeshBuffer(buffer);
			}
		}
	}
}