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
#include "CDecals.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Culling/CCullingData.h"
#include "Culling/CCullingBBoxData.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CDecals);

	CATEGORY_COMPONENT(CDecals, "Renderer", "Decals");

	CDecals::CDecals() :
		m_renderData(NULL)
	{

	}

	CDecals::~CDecals()
	{

	}

	void CDecals::initComponent()
	{
		m_renderData = m_gameObject->getEntity()->addData<CDecalRenderData>();
		m_gameObject->getEntityManager()->addRenderSystem<CDecalsRenderer>();
	}

	void CDecals::updateComponent()
	{

	}

	CObjectSerializable* CDecals::createSerializable()
	{
		return CComponentSystem::createSerializable();
	}

	void CDecals::loadSerializable(CObjectSerializable* object)
	{
		return CComponentSystem::loadSerializable(object);
	}

	void CDecals::setTexture(ITexture* texture)
	{
		m_renderData->Texture = texture;
		m_renderData->Material->setTexture(0, texture);

		core::array<CEntity*>& entities = getEntities();
		for (u32 i = 0, n = entities.size(); i < n; i++)
		{
			CEntity* entity = entities[i];
			CDecalData* decal = GET_ENTITY_DATA(entity, CDecalData);
			if (decal != NULL)
			{
				m_renderData->Material->applyMaterial(decal->MeshBuffer->getMaterial());
			}
		}
	}

	CEntity* CDecals::addDecal(
		const core::vector3df& position,
		const core::vector3df& dimension,
		const core::vector3df& normal,
		float textureRotation,
		float lifeTime,
		float distance)
	{
		// References
		// https://sourceforge.net/p/irrext/code/HEAD/tree/trunk/extensions/scene/ISceneNode/DecalSystem
		CEntity* entity = createEntity();

		CDecalData* decalData = entity->addData<CDecalData>();
		decalData->Dimension = dimension;
		decalData->Normal = normal;
		decalData->Normal.normalize();
		decalData->TextureRotation = textureRotation;
		decalData->LifeTime = lifeTime;
		decalData->Distance = distance;
		decalData->RenderData = m_renderData;

		// add transform
		CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
		transform->Relative.setTranslation(position);

		// Culling
		entity->addData<CWorldInverseTransformData>();
		entity->addData<CCullingData>();
		entity->addData<CCullingBBoxData>();

		// apply material
		m_renderData->Material->applyMaterial(decalData->MeshBuffer->getMaterial());

		// fix uv clamp
		SMaterial& mat = decalData->MeshBuffer->getMaterial();
		mat.TextureLayer[0].TextureWrapU = E_TEXTURE_CLAMP::ETC_CLAMP_TO_EDGE;
		mat.TextureLayer[0].TextureWrapV = E_TEXTURE_CLAMP::ETC_CLAMP_TO_EDGE;

		return entity;
	}

	void CDecals::bake(CCollisionBuilder* collisionMgr)
	{
		core::array<CEntity*>& entities = getEntities();
		for (u32 i = 0, n = entities.size(); i < n; i++)
		{
			CEntity* entity = entities[i];

			CDecalData* decalData = GET_ENTITY_DATA(entity, CDecalData);
			CWorldTransformData* decalTransform = GET_ENTITY_DATA(entity, CWorldTransformData);

			if (decalData && decalData->Change)
			{
				initDecal(entity, decalData, decalTransform->Relative.getTranslation(), collisionMgr);
				decalData->Change = false;
			}
		}
	}

	void CDecals::initDecal(CEntity* entity, CDecalData* decal, const core::vector3df& position, CCollisionBuilder* collision)
	{
		// Create boxes
		irr::core::aabbox3df box = irr::core::aabbox3df(
			position - decal->Dimension * 0.5f,
			position + decal->Dimension * 0.5f
		);

		// Query tris collision
		core::array<core::triangle3df*> triangles;
		core::array<CCollisionNode*> nodes;
		collision->getTriangles(box, triangles, nodes);
		u32 triangleCount = triangles.size();

		// Scale to 0.0f - 1.0f (UV space)
		core::vector3df uvScale = core::vector3df(1, 1, 1) / decal->Dimension;
		core::vector3df uvOffset(0.5f, 0.0f, 0.5f);

		// UV Rotation matrix
		core::quaternion r1;
		r1.rotationFromTo(core::vector3df(0.0f, 1.0f, 0.0f), decal->Normal);

		core::quaternion r2;
		r2.fromAngleAxis(decal->TextureRotation * core::DEGTORAD, core::vector3df(0.0f, 1.0f, 0.0f));

		core::quaternion q = r2 * r1;

		// Inverse vertex to local position
		core::matrix4 uvMatrix = q.getMatrix();
		uvMatrix.setTranslation(position);
		uvMatrix.makeInverse();

		// Position matrix
		core::matrix4 worldInverse;
		worldInverse.setTranslation(position);
		worldInverse.makeInverse();

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
					core::vector3df uvPos;
					core::vector3df pos;

					if (p == 0)
					{
						uvPos = uvTriangle.pointA * uvScale + uvOffset;
						pos = triangle.pointA;
					}
					else if (p == 1)
					{
						uvPos = uvTriangle.pointB * uvScale + uvOffset;
						pos = triangle.pointB;
					}
					else if (p == 2)
					{
						uvPos = uvTriangle.pointC * uvScale + uvOffset;
						pos = triangle.pointC;
					}

					worldInverse.transformVect(pos);

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

		CCullingBBoxData* cullingBox = GET_ENTITY_DATA(entity, CCullingBBoxData);
		cullingBox->BBox = decal->MeshBuffer->getBoundingBox();
		cullingBox->Materials.clear();
		cullingBox->Materials.push_back(m_renderData->Material);
	}
}