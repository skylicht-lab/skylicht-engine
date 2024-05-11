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
#include "CViewpointData.h"
#include "Projective/CProjective.h"
#include "Material/Shader/CShaderManager.h"
#include "TextureManager/CTextureManager.h"
#include "GameObject/CGameObject.h"
#include "Camera/CCamera.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_DATA_TYPE_INDEX(CViewpointData);

		CViewpointData::CViewpointData()
		{
			Buffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD));
			Buffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
			Buffer->getMaterial().BackfaceCulling = false;
			Buffer->setHardwareMappingHint(EHM_STREAM);

			ITexture* texture = CTextureManager::getInstance()->getTexture("Editor/Textures/oxyz.png");
			Buffer->getMaterial().setTexture(0, texture);
			Buffer->getMaterial().TextureLayer[0].BilinearFilter = false;
			Buffer->getMaterial().TextureLayer[0].TrilinearFilter = false;
			Buffer->getMaterial().TextureLayer[0].AnisotropicFilter = 8;

			const float p = 0.8f;

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

			SColor blue = SColor(255, 44, 143, 255);
			SColor red = SColor(255, 155, 65, 80);
			SColor green = SColor(255, 105, 140, 45);
			core::vector3df zero(0.0f, 0.0f, 0.0f);

			float s = 0.8f;
			addLine(zero, Position[0] * s, red);
			addLine(zero, Position[2] * s, blue);
			addLine(zero, Position[4] * s, green);
		}

		CViewpointData::~CViewpointData()
		{
			Buffer->drop();
		}

		void CViewpointData::updateBillboard(const core::vector3df& look, const core::vector3df& up, const core::vector3df& campos)
		{
			video::S3DVertex* vtx = (video::S3DVertex*)Buffer->getVertexBuffer()->getVertices();

			core::vector3df upQuad = up;
			core::vector3df sideQuad = upQuad.crossProduct(look);

			upQuad.normalize();
			sideQuad.normalize();

			upQuad *= 0.25f;
			sideQuad *= 0.25f;

			SColor color(255, 255, 255, 255);
			core::vector2df uvScale(0.5f, 0.5f);
			core::vector2df uvOffset(0.0f, 0.0f);

			// sort to fix alpha depth
			m_sortPosition.clear();
			for (int i = 0; i < 6; i++)
			{
				float d = Position[i].getDistanceFromSQ(campos);
				m_sortPosition.push_back(std::pair<int, float>(i, d));
			}
			struct {
				bool operator()(const std::pair<int, float>& a, const std::pair<int, float>& b) const { return a.second > b.second; }
			} customLess;
			std::sort(m_sortPosition.begin(), m_sortPosition.end(), customLess);

			for (int i = 0; i < 6; i++)
			{
				int offset = i * 4;

				int id = m_sortPosition[i].first;

				float x = Position[id].X;
				float y = Position[id].Y;
				float z = Position[id].Z;

				if (id == 1)
				{
					// -x
					uvOffset.set(0.5f, 0.5f);
					color.set(255, 154, 57, 71);
				}
				else if (id == 3)
				{
					// -y
					uvOffset.set(0.5f, 0.5f);
					color.set(255, 52, 100, 154);
				}
				else if (id == 5)
				{
					// -z
					uvOffset.set(0.5f, 0.5f);
					color.set(255, 98, 138, 34);
				}
				else if (id == 0)
				{
					// x
					uvOffset.set(0.0f, 0.0f);
					color.set(255, 255, 255, 255);
				}
				else if (id == 2)
				{
					// y
					uvOffset.set(0.0f, 0.5f);
					color.set(255, 255, 255, 255);
				}
				else if (id == 4)
				{
					// z
					uvOffset.set(0.5f, 0.0f);
					color.set(255, 255, 255, 255);
				}

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

		CViewpointData::EAxis CViewpointData::hit(CCamera* camera, float x, float y, int viewportW, int viewportH)
		{
			video::S3DVertex* vtx = (video::S3DVertex*)Buffer->getVertexBuffer()->getVertices();

			core::vector3df camPosition = camera->getGameObject()->getPosition();
			core::line3df ray = CProjective::getViewRay(camera, x, y, viewportW, viewportH);

			int result = -1;
			float minD = FLT_MAX;

			for (int i = 0; i < 6; i++)
			{
				int id = i * 4;

				core::vector3df itersecion;
				core::triangle3df tri;

				tri.pointA = vtx[id].Pos;
				tri.pointB = vtx[id + 1].Pos;
				tri.pointC = vtx[id + 2].Pos;

				if (tri.getIntersectionWithLimitedLine(ray, itersecion))
				{
					float d = camPosition.getDistanceFromSQ(itersecion);
					if (d < minD)
						result = i;
				}

				tri.pointA = vtx[id].Pos;
				tri.pointB = vtx[id + 2].Pos;
				tri.pointC = vtx[id + 3].Pos;

				if (tri.getIntersectionWithLimitedLine(ray, itersecion))
				{
					float d = camPosition.getDistanceFromSQ(itersecion);
					if (d < minD)
						result = i;
				}
			}

			if (result >= 0)
			{
				int id = m_sortPosition[result].first;
				return (EAxis)id;
			}

			return None;
		}
	}
}