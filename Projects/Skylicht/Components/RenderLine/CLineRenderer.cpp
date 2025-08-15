#include "pch.h"
#include "CLineRenderer.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "IndirectLighting/CIndirectLightingData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CLineRenderer::CLineRenderer() :
		m_group(NULL)
	{

	}

	CLineRenderer::~CLineRenderer()
	{

	}

	void CLineRenderer::beginQuery(CEntityManager* entityManager)
	{
		m_transforms.reset();
		m_lines.reset();

		if (m_group == NULL)
		{
			const u32 line[] = GET_LIST_ENTITY_DATA(CLineRenderData);
			m_group = entityManager->createGroupFromVisible(line, 1);
		}
	}

	void CLineRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CLineRenderData* l = GET_ENTITY_DATA(entity, CLineRenderData);

			int numPoints = l->Points.size();
			CWorldTransformData** points = l->Points.pointer();

			for (int i = 0; i < numPoints; i++)
			{
				if (points[i]->NeedValidate)
				{
					l->NeedValidate = true;
					break;
				}
			}

			CCullingData* cullingData = GET_ENTITY_DATA(entity, CCullingData);
			if (cullingData->Visible)
			{
				if (numPoints >= 2)
				{
					m_lines.push(l);

					CWorldTransformData* t = GET_ENTITY_DATA(entity, CWorldTransformData);
					m_transforms.push(t);
				}
			}
		}
	}

	void CLineRenderer::init(CEntityManager* entityManager)
	{

	}

	void CLineRenderer::update(CEntityManager* entityManager)
	{
		CCamera* camera = entityManager->getCamera();

		CLineRenderData** lines = m_lines.pointer();
		for (int i = 0, n = (int)m_lines.count(); i < n; i++)
		{
			CLineRenderData* l = lines[i];

			if (l->NeedValidate)
			{
				updateBuffer(l, camera);
				l->NeedValidate = false;
			}
		}
	}

	void CLineRenderer::updateBuffer(CLineRenderData* line, CCamera* camera)
	{
		IVertexBuffer* buffer = line->MeshBuffer->getVertexBuffer(0);
		IIndexBuffer* index = line->MeshBuffer->getIndexBuffer();

		int indexID = 0;
		int vertexID = 0;

		// camera
		core::vector3df campos = camera->getGameObject()->getPosition();
		core::vector3df up = camera->getUpVector();

		int numPoint = line->Points.size();
		int numVertex = 4 * numPoint;
		int numIndex = 6 * numPoint;

		buffer->set_used(numVertex);
		index->set_used(numIndex);

		S3DVertex* vertices = (S3DVertex*)buffer->getVertices();
		u32* indices = (u32*)index->getIndices();

		float currentLength = 0.0f;
		float length = 0.0f;

		CWorldTransformData** transforms = line->Points.pointer();
		core::array<core::vector3df> points;

		for (int i = 0; i < numPoint - 1; i++)
		{
			core::vector3df pos1 = transforms[i]->Relative.getTranslation();
			core::vector3df pos2 = transforms[i + 1]->Relative.getTranslation();

			length = length + pos1.getDistanceFrom(pos2);

			points.push_back(pos1);
			if (i == numPoint - 2)
				points.push_back(pos2);
		}

		SColor c(255, 255, 255, 255);
		float thickness = line->Width;

		// build trail mesh buffer
		for (int i = 0; i < numPoint - 1; i++)
		{
			const core::vector3df& pos1 = points[i];
			const core::vector3df& pos2 = points[i + 1];

			// direction
			core::vector3df direction = pos1 - pos2;

			float uv1 = currentLength / length;

			// length
			currentLength = currentLength + direction.getLength();

			direction.normalize();

			float uv2 = currentLength / length;

			// look
			core::vector3df lookdir = pos1 - campos;
			if (lookdir.getLength() < 0.2f)
				continue;
			lookdir.normalize();

			// view angle
			f32 angle = lookdir.dotProduct(direction);

			if (angle < 0.9999f && angle > -0.9999f)
			{
				core::vector3df updown = direction.crossProduct(lookdir);
				updown.normalize();

				// note: we use updown as normal on billboard
				// core::vector3df normal = direction.crossProduct(updown);

				u32 vertex = i * 4;
				u32 index = i * 6;

				// vertex buffer
				vertices[vertex + 0].Pos = pos1 - updown * thickness * 0.5f;
				vertices[vertex + 0].Normal = updown;
				vertices[vertex + 0].Color = c;
				vertices[vertex + 0].TCoords.set(0.0f, uv1);

				vertices[vertex + 1].Pos = pos1 + updown * thickness * 0.5f;
				vertices[vertex + 1].Normal = updown;
				vertices[vertex + 1].Color = c;
				vertices[vertex + 1].TCoords.set(1.0f, uv1);

				vertices[vertex + 2].Pos = pos2 - updown * thickness * 0.5f;
				vertices[vertex + 2].Normal = updown;
				vertices[vertex + 2].Color = c;
				vertices[vertex + 2].TCoords.set(0.0f, uv2);

				vertices[vertex + 3].Pos = pos2 + updown * thickness * 0.5f;
				vertices[vertex + 3].Normal = updown;
				vertices[vertex + 3].Color = c;
				vertices[vertex + 3].TCoords.set(1.0f, uv2);

				// index buffer
				indices[index + 0] = vertex + 0;
				indices[index + 1] = vertex + 1;
				indices[index + 2] = vertex + 2;

				indices[index + 3] = vertex + 1;
				indices[index + 4] = vertex + 3;
				indices[index + 5] = vertex + 2;
			}
		}

		buffer->setDirty();
		index->setDirty();
	}

	void CLineRenderer::render(CEntityManager* entityManager)
	{
		CWorldTransformData** transforms = m_transforms.pointer();
		CLineRenderData** lines = m_lines.pointer();
		int n = m_lines.count();

		for (int i = 0; i < n; i++)
		{
			renderLine(entityManager, transforms[i], lines[i]);
		}
	}

	void CLineRenderer::renderTransparent(CEntityManager* entityManager)
	{

	}

	void CLineRenderer::renderLine(CEntityManager* entityMgr, CWorldTransformData* transform, CLineRenderData* line)
	{
		IVideoDriver* driver = getVideoDriver();

		driver->setTransform(video::ETS_WORLD, transform->World);

		CShaderMaterial::setMaterial(line->Material);

		CIndirectLightingData* lightingData = GET_ENTITY_DATA(line->Entity, CIndirectLightingData);
		if (lightingData != NULL)
			lightingData->applyShader();

		CMesh* mesh = line->Mesh;
		mesh->Materials[0] = line->Material;
		entityMgr->getRenderPipeline()->drawMeshBuffer(mesh, 0, entityMgr, line->EntityIndex, false);
		mesh->Materials[0] = NULL;
	}
}