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
#include "CCPURenderer.h"
#include "Material/Shader/ShaderCallback/CShaderParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		CCPURenderer::CCPURenderer() :
			IRenderer(CPURenderer),
			m_billboardType(Billboard),
			m_transparentType(Addtive)
		{
			m_useInstancing = false;

			m_material = new CMaterial("Particle", "BuiltIn/Shader/Particle/ParticleAdditive.xml");
			m_material->setBackfaceCulling(false);
			m_material->setZWrite(false);

			setTexturePath("BuiltIn/Textures/NullTexture.png");
		}

		CCPURenderer::~CCPURenderer()
		{
			m_material->drop();
		}

		void CCPURenderer::getParticleBuffer(IMeshBuffer* buffer)
		{
			// no need update buffer, just update material
			m_needUpdateMesh = false;
			m_material->applyMaterial();
		}

		void CCPURenderer::updateParticleBuffer(IMeshBuffer* buffer, CParticle* particles, int num)
		{
			IVertexBuffer* vtx = buffer->getVertexBuffer();
			IIndexBuffer* idx = buffer->getIndexBuffer();

			vtx->set_used(NB_VERTICES_PER_QUAD * num);
			idx->set_used(NB_INDICES_PER_QUAD * num);

			video::S3DVertex* vertices = (video::S3DVertex*)vtx->getVertices();

			CParticle* p;
			float* params;
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

			bool isIndices32 = idx->getType() == irr::video::EIT_32BIT;
			irr::u32* indices32 = reinterpret_cast<irr::u32*>(idx->getIndices());
			irr::u16* indices16 = reinterpret_cast<irr::u16*>(idx->getIndices());

			for (int i = 0; i < num; i++)
			{
				p = particles + i;
				params = p->Params;

				float sx = SizeX * params[ScaleX] * 0.5f;
				float sy = SizeY * params[ScaleY] * 0.5f;

				if (m_billboardType == Frontal)
				{
					look = p->Velocity;
					look.normalize();
					sideQuad.set(0.0f, 0.0f, 1.0f);

					f32 angle = look.dotProduct(sideQuad);
					if (angle < 0.9999f && angle > -0.9999f)
						sideQuad.set(1.0f, 0.0f, 0.0f);
					else
						sideQuad.set(0.0f, 0.0f, 1.0f);

					up = look.crossProduct(sideQuad);
				}
				else if (m_billboardType == Velocity)
				{
					up = p->Velocity;
					up.normalize();
				}

				if (m_billboardType == Billboard ||
					m_billboardType == Frontal ||
					m_billboardType == Velocity)
				{
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
				}
				else if (m_billboardType == RotateY)
				{
					upQuad.set(0.0f, 1.0f, 0.0f);
				}

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
				if (isIndices32)
				{
					indices32[offset] = offsetVertex;
					indices32[offset + 1] = offsetVertex + 1;
					indices32[offset + 2] = offsetVertex + 2;

					indices32[offset + 3] = offsetVertex;
					indices32[offset + 4] = offsetVertex + 2;
					indices32[offset + 5] = offsetVertex + 3;
				}
				else
				{
					indices16[offset] = offsetVertex;
					indices16[offset + 1] = offsetVertex + 1;
					indices16[offset + 2] = offsetVertex + 2;

					indices16[offset + 3] = offsetVertex;
					indices16[offset + 4] = offsetVertex + 2;
					indices16[offset + 5] = offsetVertex + 3;
				}
			}

			buffer->setDirty();
		}

		CObjectSerializable* CCPURenderer::createSerializable()
		{
			CObjectSerializable* object = IRenderer::createSerializable();
			CEnumProperty<EBillboardType>* billboardType = new CEnumProperty<EBillboardType>(object, "billboardType", m_billboardType);
			billboardType->addEnumString("Billboard", EBillboardType::Billboard);
			billboardType->addEnumString("Velocity", EBillboardType::Velocity);
			billboardType->addEnumString("Frontal", EBillboardType::Frontal);
			billboardType->addEnumString("RotateY", EBillboardType::RotateY);

			CEnumProperty<ETransparentType>* transparentType = new CEnumProperty<ETransparentType>(object, "transparentType", m_transparentType);
			transparentType->addEnumString("Addtive", ETransparentType::Addtive);
			transparentType->addEnumString("Transparent", ETransparentType::Transparent);
			return object;
		}

		void CCPURenderer::loadSerializable(CObjectSerializable* object)
		{
			IRenderer::loadSerializable(object);
			m_billboardType = object->get<EBillboardType>("billboardType", EBillboardType::Billboard);

			ETransparentType transparentType = object->get<ETransparentType>("transparentType", ETransparentType::Addtive);

			if (transparentType != m_transparentType)
			{
				if (transparentType == ETransparentType::Addtive)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleAdditive.xml");
				else
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleTransparent.xml");
			}
			m_transparentType = transparentType;
		}
	}
}