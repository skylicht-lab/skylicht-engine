/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CBillboardAdditiveRenderer.h"
#include "Material/Shader/ShaderCallback/CShaderParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		CBillboardAdditiveRenderer::CBillboardAdditiveRenderer() :
			IRenderer(BillboardAddtive),
			m_atlasNx(1),
			m_atlasNy(1)
		{
			m_useInstancing = false;

			m_material = new CMaterial("Particle", "BuiltIn/Shader/Particle/ParticleAdditive.xml");
			m_material->setBackfaceCulling(false);
			m_material->setZWrite(false);
		}

		CBillboardAdditiveRenderer::~CBillboardAdditiveRenderer()
		{
			delete m_material;
		}

		void CBillboardAdditiveRenderer::setTexture(int slot, ITexture *texture)
		{
			m_material->setTexture(slot, texture);
			m_material->setManualInitTexture(true);
		}

		void CBillboardAdditiveRenderer::getParticleBuffer(IMeshBuffer *buffer)
		{
			m_material->addAffectMesh(buffer);
			m_material->applyMaterial();
		}

		void CBillboardAdditiveRenderer::updateParticleBuffer(IMeshBuffer *buffer, CParticle *particles, int num)
		{
			IVertexBuffer *vtx = buffer->getVertexBuffer();
			IIndexBuffer *idx = buffer->getIndexBuffer();

			vtx->set_used(NB_VERTICES_PER_QUAD * num);
			idx->set_used(NB_INDICES_PER_QUAD * num);

			video::S3DVertex *vertices = (video::S3DVertex*)vtx->getVertices();

			CParticle *p;
			float *params;
			u32 frame, row, col;

			u32 totalFrames = m_atlasNx * m_atlasNy;
			float frameW = 1.0f / m_atlasNx;
			float frameH = 1.0f / m_atlasNy;

			core::vector3df upQuad, sideQuad;
			core::vector3df look = CShaderParticle::getViewLook();
			core::vector3df up = CShaderParticle::getViewUp();

			float x, y, z;
			SColor color;

			core::vector2df uvScale, uvOffset;

			for (int i = 0; i < num; i++)
			{
				p = particles + i;
				params = p->Params;

				float sx = SizeX * params[ScaleX];
				float sy = SizeY * params[ScaleY];

				float rotation = p->Rotation.Z;

				float cosA = cosf(rotation);
				float sinA = sinf(rotation);

				upQuad.X = (look.X * look.X + (1.0f - look.X * look.X) * cosA) * up.X
					+ (look.X * look.Y * (1.0f - cosA) - look.Z * sinA) * up.Y
					+ (look.X * look.Z * (1.0f - cosA) + look.Y * sinA) * up.Z;

				upQuad.Y = (look.X * look.Y * (1.0f - cosA) + look.Z * sinA) * up.X
					+ (look.Y * look.Y + (1.0f - look.Y * look.Y) * cosA) * up.Y
					+ (look.Y * look.Z * (1.0f - cosA) - look.X * sinA) * up.Z;

				upQuad.Z = (look.X * look.Z * (1.0f - cosA) - look.Y * sinA) * up.X
					+ (look.Y * look.Z * (1.0f - cosA) + look.X * sinA) * up.Y
					+ (look.Z * look.Z + (1.0f - look.Z * look.Z) * cosA) * up.Z;

				sideQuad = upQuad.crossProduct(look);

				sideQuad *= sx;
				upQuad *= sy;

				x = p->Position.X;
				y = p->Position.Y;
				z = p->Position.Z;

				color.set(
					(u32)(params[ColorA] * 255.0f),
					(u32)(params[ColorR] * 255.0f),
					(u32)(params[ColorG] * 255.0f),
					(u32)(params[ColorB] * 255.0f)
				);

				frame = (u32)params[FrameIndex];
				frame = frame < 0 ? 0 : frame;
				frame = frame >= totalFrames ? totalFrames - 1 : frame;

				row = frame / m_atlasNx;
				col = frame - (row * m_atlasNy);

				uvScale.set(frameW, frameH);
				uvOffset.set(col * frameW, row * frameH);

				int offset = i * 4;
				int offsetVertex = offset;

				// top left vertex
				vertices[offset].Pos.set(
					x - sideQuad.X + upQuad.X,
					y - sideQuad.Y + upQuad.Y,
					z - sideQuad.Z + upQuad.Z);
				vertices[offset].Color = color;
				vertices[offset].TCoords = core::vector2df(0.0f, 0.0f) * uvScale + uvOffset;

				// top right vertex				
				vertices[++offset].Pos.set(
					x + sideQuad.X + upQuad.X,
					y + sideQuad.Y + upQuad.Y,
					z + sideQuad.Z + upQuad.Z);
				vertices[offset].Color = color;
				vertices[offset].TCoords = core::vector2df(1.0f, 0.0f) * uvScale + uvOffset;

				// bottom right vertex
				vertices[++offset].Pos.set(
					x + sideQuad.X - upQuad.X,
					y + sideQuad.Y - upQuad.Y,
					z + sideQuad.Z - upQuad.Z);
				vertices[offset].Color = color;
				vertices[offset].TCoords = core::vector2df(1.0f, 1.0f) * uvScale + uvOffset;

				// bottom left vertex
				vertices[++offset].Pos.set(
					x - sideQuad.X - upQuad.X,
					y - sideQuad.Y - upQuad.Y,
					z - sideQuad.Z - upQuad.Z);
				vertices[offset].Color = color;
				vertices[offset].TCoords = core::vector2df(0.0f, 1.0f) * uvScale + uvOffset;

				offset = i * 6;

				// setup indices
				if (idx->getType() == irr::video::EIT_32BIT)
				{
					irr::u32* indices = reinterpret_cast<irr::u32*>(idx->getIndices());
					indices[offset] = offsetVertex;
					indices[offset + 1] = offsetVertex + 1;
					indices[offset + 2] = offsetVertex + 2;

					indices[offset + 3] = offsetVertex;
					indices[offset + 4] = offsetVertex + 2;
					indices[offset + 5] = offsetVertex + 3;
				}
				else if (idx->getType() == irr::video::EIT_16BIT)
				{
					irr::u16* indices = reinterpret_cast<irr::u16*>(idx->getIndices());
					indices[offset] = offsetVertex;
					indices[offset + 1] = offsetVertex + 1;
					indices[offset + 2] = offsetVertex + 2;

					indices[offset + 3] = offsetVertex;
					indices[offset + 4] = offsetVertex + 2;
					indices[offset + 5] = offsetVertex + 3;
				}
			}

			buffer->setDirty();
		}
	}
}