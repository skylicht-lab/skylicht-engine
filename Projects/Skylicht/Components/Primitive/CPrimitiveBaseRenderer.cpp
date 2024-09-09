/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CPrimitiveBaseRenderer.h"
#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "Transform/CWorldTransformData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Importer/Utils/CMeshUtils.h"

namespace Skylicht
{
	CPrimitiveBaseRenderer::CPrimitiveBaseRenderer()
	{
		m_pipelineType = IRenderPipeline::Mix;

		for (int i = 0; i < (int)CPrimiviteData::Count; i++)
		{
			m_mesh[i] = NULL;
			m_meshTangent[i] = NULL;
		}

		const IGeometryCreator* geometry = getIrrlichtDevice()->getSceneManager()->getGeometryCreator();

		// cube mesh
		{
			initCube(CPrimiviteData::Cube, false);
			initCube(CPrimiviteData::Cube, true);
		}

		// sphere mesh
		{
			IMesh* m = geometry->createSphereMesh(0.5f);
			initMesh(m, CPrimiviteData::Sphere, false);
			m->drop();

			m = geometry->createSphereMesh(0.5f);
			initMesh(m, CPrimiviteData::Sphere, true);
			m->drop();
		}

		// plane
		{
			initPlane(CPrimiviteData::Plane, false);
			initPlane(CPrimiviteData::Plane, true);
		}
	}

	CPrimitiveBaseRenderer::~CPrimitiveBaseRenderer()
	{
		for (int i = 0; i < (int)CPrimiviteData::Count; i++)
		{
			if (m_mesh[i])
				m_mesh[i]->drop();

			if (m_meshTangent[i])
				m_meshTangent[i]->drop();
		}
	}

	void CPrimitiveBaseRenderer::initCube(CPrimiviteData::EPrimitive primitive, bool tangent)
	{
		IVideoDriver* driver = getVideoDriver();

		IMeshBuffer* meshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);
		IIndexBuffer* ib = meshBuffer->getIndexBuffer();
		IVertexBuffer* vb = meshBuffer->getVertexBuffer();

		// Create vertices
		video::SColor clr(255, 255, 255, 255);

		vb->reallocate(12);

		video::S3DVertex Vertices[] = {
			// back
			video::S3DVertex(0, 0, 0, 0, 0, -1, clr, 0, 1),
			video::S3DVertex(1, 0, 0, 0, 0, -1, clr, 1, 1),
			video::S3DVertex(1, 1, 0, 0, 0, -1, clr, 1, 0),
			video::S3DVertex(0, 1, 0, 0, 0, -1, clr, 0, 0),

			// front
			video::S3DVertex(0, 0, 1, 0, 0, 1, clr, 0, 1),
			video::S3DVertex(1, 0, 1, 0, 0, 1, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 0, 0, 1, clr, 1, 0),
			video::S3DVertex(0, 1, 1, 0, 0, 1, clr, 0, 0),

			// bottom
			video::S3DVertex(0, 0, 0, 0, -1, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, 0, -1, 0, clr, 1, 1),
			video::S3DVertex(1, 0, 1, 0, -1, 0, clr, 1, 0),
			video::S3DVertex(1, 0, 0, 0, -1, 0, clr, 0, 0),

			// top
			video::S3DVertex(0, 1, 0, 0, 1, 0, clr, 0, 1),
			video::S3DVertex(0, 1, 1, 0, 1, 0, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 0, 1, 0, clr, 1, 0),
			video::S3DVertex(1, 1, 0, 0, 1, 0, clr, 0, 0),

			// left
			video::S3DVertex(1, 0, 0, 1, 0, 0, clr, 0, 1),
			video::S3DVertex(1, 0, 1, 1, 0, 0, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 1, 0, 0, clr, 1, 0),
			video::S3DVertex(1, 1, 0, 1, 0, 0, clr, 0, 0),

			// right
			video::S3DVertex(0, 0, 0, -1, 0, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, -1, 0, 0, clr, 1, 1),
			video::S3DVertex(0, 1, 1, -1, 0, 0, clr, 1, 0),
			video::S3DVertex(0, 1, 0, -1, 0, 0, clr, 0, 0),
		};

		for (u32 i = 0; i < 24; ++i)
		{
			Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
			vb->addVertex(&Vertices[i]);
		}

		// cube mesh
		// Create indices
		const u16 u[36] =
		{
			// back
			0,2,1,
			0,3,2,

			// front
			4,5,6,
			4,6,7,

			// bottom
			8,10,9,
			8,11,10,

			// top
			12,13,14,
			12,14,15,

			// left
			16,18,17,
			16,19,18,

			// right
			20,21,22,
			20,22,23
		};

		ib->set_used(36);

		for (u32 i = 0; i < 36; ++i)
			ib->setIndex(i, u[i]);

		CMesh* mesh = new CMesh();

		if (tangent)
			CMeshUtils::convertToTangentVertices(meshBuffer);

		mesh->addMeshBuffer(meshBuffer);

		meshBuffer->recalculateBoundingBox();
		mesh->recalculateBoundingBox();
		mesh->setHardwareMappingHint(EHM_STATIC);

		if (tangent)
			m_meshTangent[primitive] = mesh;
		else
			m_mesh[primitive] = mesh;

		meshBuffer->drop();
	}

	void CPrimitiveBaseRenderer::initPlane(CPrimiviteData::EPrimitive primitive, bool tangent)
	{
		IVideoDriver* driver = getVideoDriver();

		IMeshBuffer* meshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);
		IIndexBuffer* ib = meshBuffer->getIndexBuffer();
		IVertexBuffer* vb = meshBuffer->getVertexBuffer();

		video::SColor clr(255, 255, 255, 255);

		vb->reallocate(4);

		video::S3DVertex Vertices[] = {
			video::S3DVertex(0, 0, 0, 0, 1, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, 0, 1, 0, clr, 1, 1),
			video::S3DVertex(1, 0, 1, 0, 1, 0, clr, 1, 0),
			video::S3DVertex(1, 0, 0, 0, 1, 0, clr, 0, 0),
		};

		for (u32 i = 0; i < 4; ++i)
		{
			Vertices[i].Pos -= core::vector3df(0.5f, 0.0f, 0.5f);
			vb->addVertex(&Vertices[i]);
		}

		// Create indices
		const u16 u[6] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		ib->set_used(6);

		for (u32 i = 0; i < 6; ++i)
			ib->setIndex(i, u[i]);

		CMesh* mesh = new CMesh();

		if (tangent)
			CMeshUtils::convertToTangentVertices(meshBuffer);

		mesh->addMeshBuffer(meshBuffer);

		meshBuffer->recalculateBoundingBox();
		mesh->recalculateBoundingBox();
		mesh->setHardwareMappingHint(EHM_STATIC);

		if (tangent)
			m_meshTangent[primitive] = mesh;
		else
			m_mesh[primitive] = mesh;

		meshBuffer->drop();
	}

	void CPrimitiveBaseRenderer::initMesh(IMesh* primitiveMesh, CPrimiviteData::EPrimitive primitive, bool tangent)
	{
		CMesh* mesh = new CMesh();

		IMeshBuffer* mb = primitiveMesh->getMeshBuffer(0);

		if (tangent)
			CMeshUtils::convertToTangentVertices(mb);

		mesh->addMeshBuffer(mb);

		mb->recalculateBoundingBox();
		mesh->recalculateBoundingBox();
		mesh->setHardwareMappingHint(EHM_STATIC);

		if (tangent)
			m_meshTangent[primitive] = mesh;
		else
			m_mesh[primitive] = mesh;
	}
}