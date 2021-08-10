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
#include <functional>

namespace Skylicht
{
	CSpriteRenderer::CSpriteRenderer()
	{
		m_meshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(video::EVT_STANDARD), video::EIT_16BIT);
		m_meshBuffer->setHardwareMappingHint(EHM_STREAM);

		m_renderPass = Transparent;
	}

	CSpriteRenderer::~CSpriteRenderer()
	{
		m_meshBuffer->drop();
	}

	void CSpriteRenderer::beginQuery()
	{
		m_sprites.set_used(0);
		m_transforms.set_used(0);
	}

	void CSpriteRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CSpriteDrawData* spriteData = entity->getData<CSpriteDrawData>();
		if (spriteData != NULL && spriteData->Frame != NULL)
		{
			CWorldTransformData* transform = entity->getData<CWorldTransformData>();
			CVisibleData* visible = entity->getData<CVisibleData>();

			if (transform != NULL && visible->Visible)
			{
				m_sprites.push_back(spriteData);
				m_transforms.push_back(transform);
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

		int vertexOffset = 0;
		video::S3DVertex* vertices = (video::S3DVertex*)vb->getVertices();
		u16* indices = (u16*)ib->getIndices();

		for (u32 i = 1, n = m_sprites.size(); i < n; i++)
		{
			SFrame* frame = m_sprites[i]->Frame;
			float scale = m_sprites[i]->Scale;

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

				module.getPositionBuffer(vertices, indices, vertexOffset, m_transforms[i]->World, scale, scale);
				module.getTexCoordBuffer(vertices, texWidth, texHeight);
				module.getColorBuffer(vertices, m_sprites[i]->Color);

				vertexOffset += 4;
				vertices += 4;
				indices += 6;

				++it;
			}

			// if change texture or last sprite
			if (i == n - 1 || currentTexture != m_sprites[i + 1]->Frame->Image->Texture)
			{
				// flush render
				m_meshBuffer->setDirty();
				driver->drawMeshBuffer(m_meshBuffer);

				// clean
				vb->set_used(0);
				ib->set_used(0);

				// reset buffer position
				vertexOffset = 0;
				vertices = (video::S3DVertex*)vb->getVertices();
				indices = (u16*)ib->getIndices();
			}
		}
	}
}