/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CSpriteRenderer.h"
#include "Culling/CVisibleData.h"
#include "Material/Shader/CShaderManager.h"
#include "Entity/CEntityManager.h"
#include "Projective/CProjective.h"
#include <functional>

namespace Skylicht
{
	CSpriteRenderer::CSpriteRenderer()
	{
		m_meshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(video::EVT_STANDARD), video::EIT_16BIT);
		m_meshBuffer->setHardwareMappingHint(EHM_STREAM);

		m_meshBuffer->getMaterial().BackfaceCulling = false;
		m_meshBuffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");

		m_renderPass = Transparent;
	}

	CSpriteRenderer::~CSpriteRenderer()
	{
		m_meshBuffer->drop();
	}

	void CSpriteRenderer::beginQuery(CEntityManager* entityManager)
	{
		m_sprites.set_used(0);
		m_transforms.set_used(0);
	}

	void CSpriteRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CSpriteDrawData* spriteData = GET_ENTITY_DATA(entity, CSpriteDrawData);
			if (spriteData != NULL && spriteData->Frame != NULL)
			{
				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);

				if (visible->Visible)
				{
					m_sprites.push_back(spriteData);
					m_transforms.push_back(transform);
				}
			}
		}
	}

	void CSpriteRenderer::init(CEntityManager* entityManager)
	{

	}

	void CSpriteRenderer::update(CEntityManager* entityManager)
	{

	}

	void CSpriteRenderer::render(CEntityManager* entityManager)
	{

	}

	void CSpriteRenderer::renderTransparent(CEntityManager* entityManager)
	{
		if (m_sprites.size() == 0)
			return;

		IVertexBuffer* vb = m_meshBuffer->getVertexBuffer();
		IIndexBuffer* ib = m_meshBuffer->getIndexBuffer();

		// insertion sort sprite by texture
		for (u32 i = 1, n = m_sprites.size(); i < n; i++)
		{
			CSpriteDrawData* sprite = m_sprites[i];
			CWorldTransformData* transform = m_transforms[i];

			u32 j = i - 1;

			std::function<int(CSpriteDrawData*, CSpriteDrawData*)> compare = [](CSpriteDrawData* a, CSpriteDrawData* b) {
				if (a->Frame->Image->Texture == b->Frame->Image->Texture)
				{
					return 0;
				}
				else if (a->Frame->Image->Texture > b->Frame->Image->Texture)
				{
					return 1;
				}
				else
				{
					return -1;
				}
				return 0;
			};

			while (j >= 0 && compare(sprite, m_sprites[j]) > 0)
			{
				m_sprites[j + 1] = m_sprites[j];
				m_transforms[j + 1] = m_transforms[j];
				--j;
			}

			m_sprites[j + 1] = sprite;
			m_transforms[j + 1] = transform;
		}

		ITexture* currentTexture = NULL;
		vb->set_used(0);
		ib->set_used(0);

		IVideoDriver* driver = getVideoDriver();

		int numVertex = 0;
		int numIndex = 0;

		irr::core::matrix4 invModelView;
		{
			irr::core::matrix4 modelView(driver->getTransform(video::ETS_VIEW));
			modelView.getInversePrimitive(invModelView); // wont work for odd modelview matrices (but should happen in very special cases)
		}

		core::vector3df right(invModelView[0], invModelView[1], invModelView[2]);
		core::vector3df look(invModelView[8], invModelView[9], invModelView[10]);
		core::vector3df up(invModelView[4], invModelView[5], invModelView[6]);

		right.normalize();
		look.normalize();
		up.normalize();

		core::matrix4 transform;
		f32* matData = transform.pointer();
		matData[0] = right.X;
		matData[1] = right.Y;
		matData[2] = right.Z;
		matData[3] = 0.0f;

		matData[4] = up.X;
		matData[5] = up.Y;
		matData[6] = up.Z;
		matData[7] = 0.0f;

		matData[8] = look.X;
		matData[9] = look.Y;
		matData[10] = look.Z;
		matData[11] = 0.0f;

		CCamera* camera = entityManager->getCamera();

		for (u32 i = 0, n = m_sprites.size(); i < n; i++)
		{
			SFrame* frame = m_sprites[i]->Frame;

			// position
			core::vector3df pos = m_transforms[i]->World.getTranslation();

			// scale
			float scale = m_sprites[i]->Scale;
			m_sprites[i]->ViewScale = scale;
			if (m_sprites[i]->AutoScaleInViewSpace)
			{
				scale = scale * 0.0015f / CProjective::getSegmentLengthClipSpace(camera, pos, pos + right);
				m_sprites[i]->ViewScale = scale;
			}

			// render buffer
			currentTexture = frame->Image->Texture;

			core::dimension2du size = frame->Image->Texture->getSize();
			float texWidth = (float)size.Width;
			float texHeight = (float)size.Height;

			// batch sprite
			std::vector<SModuleOffset>::iterator it = frame->ModuleOffset.begin(), end = frame->ModuleOffset.end();
			while (it != end)
			{
				SModuleOffset& module = (*it);

				vb->set_used(numVertex + 4);
				ib->set_used(numIndex + 6);

				video::S3DVertex* vertices = (video::S3DVertex*)vb->getVertices();
				vertices += numVertex;

				u16* indices = (u16*)ib->getIndices();
				indices += numIndex;

				// center
				float offsetX = 0.0f;
				float offsetY = 0.0f;

				if (m_sprites[i]->Center)
				{
					offsetX = module.Module->W * 0.5f * scale;
					offsetY = module.Module->H * 0.5f * scale;
				}

				if (m_sprites[i]->Billboard)
				{
					matData[12] = pos.X;
					matData[13] = pos.Y;
					matData[14] = pos.Z;
					matData[15] = 1.0f;

					module.getPositionBuffer(vertices, indices, numVertex, -offsetX, offsetY, transform, scale, -scale);
				}
				else
				{
					module.getPositionBuffer(vertices, indices, numVertex, -offsetX, offsetY, m_transforms[i]->World, scale, -scale);
				}

				module.getTexCoordBuffer(vertices, texWidth, texHeight);
				module.getColorBuffer(vertices, m_sprites[i]->Color);

				numVertex += 4;
				numIndex += 6;

				++it;
			}

			// if change texture or last sprite
			if (i == n - 1 || currentTexture != m_sprites[i + 1]->Frame->Image->Texture)
			{
				// flush render
				m_meshBuffer->getMaterial().setTexture(0, currentTexture);
				m_meshBuffer->setDirty();

				driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
				driver->setMaterial(m_meshBuffer->getMaterial());
				driver->drawMeshBuffer(m_meshBuffer);

				// clean
				vb->set_used(0);
				ib->set_used(0);

				numVertex = 0;
				numIndex = 0;
			}
		}
	}
}