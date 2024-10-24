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
#include "CGizmosRenderer.h"

#include "Entity/CEntityManager.h"
#include "Projective/CProjective.h"

namespace Skylicht
{
	namespace Editor
	{
		CGizmosRenderer::CGizmosRenderer() :
			m_enable(true)
		{
			m_data = new CGizmosData();
		}

		CGizmosRenderer::~CGizmosRenderer()
		{
			delete m_data;
		}

		void CGizmosRenderer::init(CEntityManager* entityManager)
		{

		}

		void CGizmosRenderer::beginQuery(CEntityManager* entityManager)
		{

		}


		void CGizmosRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{

		}

		void CGizmosRenderer::drawArrowInViewSpace(const core::vector3df& pos, const core::vector3df& v, float length, float arrowSize, const SColor& color)
		{
			m_arrows.push_back(SLineArrow());

			SLineArrow& arrow = m_arrows.getLast();
			arrow.Position = pos;
			arrow.Vector = v;
			arrow.Length = length;
			arrow.ArrowSize = arrowSize;
			arrow.Color = color;
		}

		void CGizmosRenderer::drawRectBillboard(const core::vector3df& pos, float width, float height, const SColor& color)
		{
			m_rectBillboard.push_back(SRectBillboard());

			SRectBillboard& billboard = m_rectBillboard.getLast();
			billboard.Position = pos;
			billboard.Width = width;
			billboard.Height = height;
			billboard.Color = color;
		}

		void CGizmosRenderer::update(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			if (entityManager->getRenderPipeline()->getType() == IRenderPipeline::ShadowMap)
				return;

			irr::core::matrix4 invView;
			{
				irr::core::matrix4 view(getVideoDriver()->getTransform(video::ETS_VIEW));
				view.getInversePrimitive(invView);
			}

			core::vector3df cameraRight(invView[0], invView[1], invView[2]);
			core::vector3df cameraLook(invView[8], invView[9], invView[10]);
			core::vector3df cameraUp(invView[4], invView[5], invView[6]);
			core::vector3df cameraPos = entityManager->getCamera()->getGameObject()->getPosition();

			cameraRight.normalize();
			cameraLook.normalize();
			cameraUp.normalize();

			CCamera* camera = entityManager->getCamera();

			// draw arrow
			for (u32 i = 0, n = m_arrows.size(); i < n; i++)
			{
				SLineArrow& arrow = m_arrows[i];
				float length = arrow.Length / CProjective::getSegmentLengthClipSpace(camera, arrow.Position, arrow.Position + cameraRight);

				core::vector3df end = arrow.Position + arrow.Vector * length;

				// add line
				m_data->addLine(arrow.Position, end, arrow.Color);

				// add arrow
				float arrowSize1 = length * arrow.ArrowSize;
				float arrowSize2 = arrowSize1 * 0.5f;

				core::vector3df side = arrow.Vector.crossProduct(cameraLook);
				side.normalize();

				core::vector3df a = end;
				core::vector3df m = a - arrow.Vector * arrowSize1;
				core::vector3df b = m + side * arrowSize2;
				core::vector3df c = m - side * arrowSize2;
				m_data->addTriangleFill(a, b, c, arrow.Color);
			}

			// draw rect billboard
			for (u32 i = 0, n = m_rectBillboard.size(); i < n; i++)
			{
				SRectBillboard& r = m_rectBillboard[i];

				core::vector3df side = cameraUp.crossProduct(cameraLook);
				side.normalize();

				core::vector3df sideQuad = side * r.Width;
				core::vector3df upQuad = cameraUp * r.Height;

				core::vector3df v1, v2, v3, v4;
				v1.set(
					r.Position.X - sideQuad.X + upQuad.X,
					r.Position.Y - sideQuad.Y + upQuad.Y,
					r.Position.Z - sideQuad.Z + upQuad.Z
				);
				v2.set(
					r.Position.X + sideQuad.X + upQuad.X,
					r.Position.Y + sideQuad.Y + upQuad.Y,
					r.Position.Z + sideQuad.Z + upQuad.Z
				);
				v3.set(
					r.Position.X + sideQuad.X - upQuad.X,
					r.Position.Y + sideQuad.Y - upQuad.Y,
					r.Position.Z + sideQuad.Z - upQuad.Z
				);
				v4.set(
					r.Position.X - sideQuad.X - upQuad.X,
					r.Position.Y - sideQuad.Y - upQuad.Y,
					r.Position.Z - sideQuad.Z - upQuad.Z
				);

				core::vector3df quad[] = { v1, v2, v3, v4 };
				m_data->addPolygonFill(quad, 4, r.Color);
			}

			// update buffer
			((CLineDrawData*)m_data)->updateBuffer();
			((CPolygonDrawData*)m_data)->updateBuffer();

			m_arrows.set_used(0);
		}

		void CGizmosRenderer::render(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			IMeshBuffer* buffer = NULL;

			buffer = m_data->PolygonBuffer;
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);

			buffer = m_data->LineBuffer;
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);

			((CLineDrawData*)m_data)->clearBuffer();
			((CPolygonDrawData*)m_data)->clearBuffer();
		}
	}
}