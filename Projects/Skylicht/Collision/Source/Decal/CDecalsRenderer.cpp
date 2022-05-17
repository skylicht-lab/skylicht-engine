/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CDecalsRenderer.h"
#include "Culling/CVisibleData.h"

namespace Skylicht
{
	CDecalsRenderer::CDecalsRenderer()
	{

	}

	CDecalsRenderer::~CDecalsRenderer()
	{

	}

	void CDecalsRenderer::beginQuery(CEntityManager* entityManager)
	{
		m_decalRenders.set_used(0);
		m_transforms.set_used(0);

		m_decalData.set_used(0);
		m_decalTransforms.set_used(0);
	}

	void CDecalsRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CDecalRenderData* renderData = (CDecalRenderData*)entity->getDataByIndex(CDecalRenderData::DataTypeIndex);
		if (renderData != NULL)
		{
			CVisibleData* visibleData = (CVisibleData*)entity->getDataByIndex(CVisibleData::DataTypeIndex);
			if (visibleData->Visible)
			{
				CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
				m_transforms.push_back(transform);
				m_decalRenders.push_back(renderData);
			}
		}
		else
		{
			CDecalData* decalData = (CDecalData*)entity->getDataByIndex(CDecalData::DataTypeIndex);
			if (decalData != NULL)
			{
				CVisibleData* visibleData = (CVisibleData*)entity->getDataByIndex(CVisibleData::DataTypeIndex);
				if (visibleData->Visible)
				{
					CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
					m_decalTransforms.push_back(transform);
					m_decalData.push_back(decalData);
				}
			}
		}
	}

	void CDecalsRenderer::init(CEntityManager* entityManager)
	{

	}

	void CDecalsRenderer::update(CEntityManager* entityManager)
	{
		u32 numDecal = m_decalData.size();

		// create decal buffer
		CDecalData** decalDatas = m_decalData.pointer();
		CWorldTransformData** decalTransform = m_decalTransforms.pointer();

		for (u32 i = 0; i < numDecal; i++)
		{
			CDecalData* decalData = decalDatas[i];
			if (decalData->Change == true && decalData->Collision)
			{
				initDecal(decalData, decalTransform[i]->World.getTranslation());
				decalData->Change = false;
			}
		}
	}

	void CDecalsRenderer::render(CEntityManager* entityManager)
	{
		u32 numDecal = m_decalData.size();

		// create decal buffer
		CDecalData** decalDatas = m_decalData.pointer();
		CWorldTransformData** decalTransform = m_decalTransforms.pointer();

		IVideoDriver* videoDriver = getVideoDriver();

		// reset world transform
		videoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		for (u32 i = 0; i < numDecal; i++)
		{
			CDecalData* decal = decalDatas[i];

			video::SMaterial& mat = decal->MeshBuffer->getMaterial();

			videoDriver->setMaterial(mat);
			videoDriver->drawMeshBuffer(decal->MeshBuffer);
		}
	}

	void CDecalsRenderer::initDecal(CDecalData* decal, const core::vector3df& position)
	{
		// Create boxes
		irr::core::aabbox3df box = irr::core::aabbox3df(
			position - decal->Dimension * 0.5f,
			position + decal->Dimension * 0.5f
		);

		// Query tris collision
		core::array<core::triangle3df*> triangles;
		core::array<CCollisionNode*> nodes;
		decal->Collision->getTriangles(box, triangles, nodes);
		u32 triangleCount = triangles.size();

		// Scale to 0.0f - 1.0f (UV space)
		core::vector3df uvScale = core::vector3df(1, 1, 1) / decal->Dimension;
		core::vector3df uvOffset(0.5f, 0.0f, 0.5f);

		// UV Rotation matrix
		core::quaternion r1;
		r1.rotationFromTo(core::vector3df(0.0f, 1.0f, 0.0f), decal->Normal);

		core::quaternion r2;
		r2.fromAngleAxis(-decal->TextureRotation * core::DEGTORAD, core::vector3df(0.0f, 1.0f, 0.0f));

		core::quaternion q = r2 * r1;

		core::matrix4 uvMatrix = q.getMatrix();
		uvMatrix.setTranslation(position);

		uvMatrix.makeInverse();

		// Clip all triangles and fill vertex and indices
		u32 vertexIndex = 0;
		std::map<core::vector3df, u32> positions;

		IIndexBuffer* indices = decal->MeshBuffer->getIndexBuffer();
		IVertexBuffer* vertices = decal->MeshBuffer->getVertexBuffer();

		for (u32 i = 0; i < triangleCount; i++)
		{
			u32 index = 0;

			core::triangle3df uvTriangle = *triangles[i];
			core::triangle3df& triangle = *triangles[i];

			core::vector3df triangleNormal = triangle.getNormal();
			triangleNormal.normalize();

			// Rotate positions
			uvMatrix.transformVect(uvTriangle.pointA);
			uvMatrix.transformVect(uvTriangle.pointB);
			uvMatrix.transformVect(uvTriangle.pointC);


			// Fill vertices and indices
			{
				video::SColor color(255, 255, 255, 255);

				for (u32 p = 0; p < 3; p++)
				{
					core::vector3df uvPos = uvTriangle.pointA * uvScale + uvOffset;
					core::vector3df pos = triangle.pointA;

					if (p == 1)
					{
						uvPos = uvTriangle.pointB * uvScale + uvOffset;
						pos = triangle.pointB;
					}
					else if (p == 2)
					{
						uvPos = uvTriangle.pointC * uvScale + uvOffset;
						pos = triangle.pointC;
					}

					// Search if vertex already exists in the vertices list
					std::map<core::vector3df, u32>::iterator iter = positions.find(uvPos);
					if (iter != positions.end())
					{
						index = iter->second;
					}
					// Add vertex to list
					else
					{
						index = vertexIndex;
						positions.insert(std::pair<core::vector3df, u32>(uvPos, vertexIndex));

						pos += triangleNormal * decal->Distance;

						core::vector2df uv(uvPos.X, 1.0f - uvPos.Z);

						video::S3DVertex vtx(
							pos,
							triangleNormal,
							color,
							uv);

						vertices->addVertex(&vtx);
						vertexIndex++;
					}

					indices->addIndex(index);
				}
			}
		}

		decal->MeshBuffer->recalculateBoundingBox();
		decal->MeshBuffer->setDirty();
	}
}