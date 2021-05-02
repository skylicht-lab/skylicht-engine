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
#include "CViewpointData.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	namespace Editor
	{
		CViewpointData::CViewpointData()
		{
			Buffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD));
			Buffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("VertexColor");
			Buffer->getMaterial().BackfaceCulling = false;
			Buffer->setHardwareMappingHint(EHM_STREAM);

			const float p = 1.0f;

			Position[0].set(p, 0.0f, 0.0f);
			Position[1].set(-p, 0.0f, 0.0f);

			Position[2].set(0.0f, p, 0.0f);
			Position[3].set(0.0f, -p, 0.0f);

			Position[4].set(0.0f, 0.0f, p);
			Position[5].set(0.0f, 0.0f, -p);

			Buffer->getVertexBuffer()->set_used(6 * 4);
			Buffer->getIndexBuffer()->set_used(6 * 2 * 3);

			u16* idx = (u16*)Buffer->getIndexBuffer()->getIndices();
			for (int i = 0; i < 6; i++)
			{
				idx[i * 6 + 0] = i * 4;
				idx[i * 6 + 1] = i * 4 + 1;
				idx[i * 6 + 2] = i * 4 + 2;

				idx[i * 6 + 3] = i * 4;
				idx[i * 6 + 4] = i * 4 + 2;
				idx[i * 6 + 5] = i * 4 + 3;
			}

			SColor blue = SColor(255, 65, 105, 140);
			SColor red = SColor(255, 155, 65, 80);
			SColor green = SColor(255, 105, 140, 45);
			core::vector3df zero(0.0f, 0.0f, 0.0f);
			
			float s = 0.7f;
			addLineVertexBatch(zero, Position[0] * s, red);
			addLineVertexBatch(zero, Position[2] * s, blue);
			addLineVertexBatch(zero, Position[4] * s, green);
		}

		CViewpointData::~CViewpointData()
		{

		}

		void CViewpointData::updateBillboard(const core::vector3df& look, const core::vector3df& up)
		{
			video::S3DVertex* vtx = (video::S3DVertex*)Buffer->getVertexBuffer()->getVertices();

			core::vector3df upQuad = up;
			core::vector3df sideQuad = upQuad.crossProduct(look);

			upQuad.normalize();
			sideQuad.normalize();

			upQuad *= 0.25f;
			sideQuad *= 0.25f;

			SColor color(255, 255, 255, 255);
			core::vector2df uvScale(1.0f, 1.0f);
			core::vector2df uvOffset(0.0f, 0.0f);

			for (int i = 0; i < 6; i++)
			{
				int offset = i * 4;
				int offsetVertex = offset;

				float x = Position[i].X;
				float y = Position[i].Y;
				float z = Position[i].Z;

				// top left vertex
				vtx[offset].Pos.set(
					x - sideQuad.X + upQuad.X,
					y - sideQuad.Y + upQuad.Y,
					z - sideQuad.Z + upQuad.Z);
				vtx[offset].Color = color;
				vtx[offset].TCoords = core::vector2df(0.0f, 0.0f) * uvScale + uvOffset;

				// top right vertex				
				vtx[++offset].Pos.set(
					x + sideQuad.X + upQuad.X,
					y + sideQuad.Y + upQuad.Y,
					z + sideQuad.Z + upQuad.Z);
				vtx[offset].Color = color;
				vtx[offset].TCoords = core::vector2df(1.0f, 0.0f) * uvScale + uvOffset;

				// bottom right vertex
				vtx[++offset].Pos.set(
					x + sideQuad.X - upQuad.X,
					y + sideQuad.Y - upQuad.Y,
					z + sideQuad.Z - upQuad.Z);
				vtx[offset].Color = color;
				vtx[offset].TCoords = core::vector2df(1.0f, 1.0f) * uvScale + uvOffset;

				// bottom left vertex
				vtx[++offset].Pos.set(
					x - sideQuad.X - upQuad.X,
					y - sideQuad.Y - upQuad.Y,
					z - sideQuad.Z - upQuad.Z);
				vtx[offset].Color = color;
				vtx[offset].TCoords = core::vector2df(0.0f, 1.0f) * uvScale + uvOffset;
			}

			Buffer->setDirty(EBT_VERTEX);
		}
	}
}