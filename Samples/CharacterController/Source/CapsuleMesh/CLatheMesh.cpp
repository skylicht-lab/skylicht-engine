#include "pch.h"
#include "CLatheMesh.h"
#include "Material/CMaterial.h"
#include "Debug/CSceneDebug.h"

// uv: https://catlikecoding.com/unity/tutorials/procedural-meshes/uv-sphere/

CLatheMesh::CLatheMesh() :
	m_mesh(NULL)
{
}

CLatheMesh::~CLatheMesh()
{
	if (m_mesh)
		m_mesh->drop();
}

void CLatheMesh::initLatheMesh(CMaterial* material)
{
	int numPoint = (int)m_outline.Points.size();
	if (numPoint == 0)
		return;

	if (m_mesh)
		m_mesh->drop();

	m_mesh = new CMesh();

	int step = 20;
	float angleStep = 2.0f * core::PI / (float)step;
	float angle = 0.0f;

	IVideoDriver* driver = getVideoDriver();
	IMeshBuffer* meshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);
	IIndexBuffer* ib = meshBuffer->getIndexBuffer();
	IVertexBuffer* vb = meshBuffer->getVertexBuffer();

	m_outline.VertexCount.clear();

	// init vertex buffer
	for (int j = 0; j < numPoint; j++)
	{
		core::vector3df& p = m_outline.Points[j];
		core::vector3df& n = m_outline.Normals[j];

		S3DVertex v;
		v.Color.set(255, 255, 255, 255);
		if (fabsf(p.Z) < FLT_EPSILON)
		{
			// center point
			v.Pos = p;
			v.Normal = n;
			vb->addVertex(&v);
			m_outline.VertexCount.push_back(1);
		}
		else
		{
			core::quaternion rot;
			core::vector3df axis(0.0f, 1.0f, 0.0f);

			for (int i = 0; i < step; i++)
			{
				rot.fromAngleAxis(angleStep * i, axis);

				v.Pos = rot * p;
				v.Normal = rot * n;
				v.Normal.normalize();
				vb->addVertex(&v);
			}

			m_outline.VertexCount.push_back(step);
		}
	}

	// init index buffer
	int vId = 0;
	for (int j = 0; j < numPoint - 1; j++)
	{
		int c1 = m_outline.VertexCount[j];
		int c2 = m_outline.VertexCount[j + 1];

		if (c1 == 1)
		{
			for (int i = 0; i < c2 - 1; i++)
			{
				ib->addIndex(vId);
				ib->addIndex(vId + c1 + i);
				ib->addIndex(vId + c1 + i + 1);
			}

			ib->addIndex(vId);
			ib->addIndex(vId + c1 + c2 - 1);
			ib->addIndex(vId + c1);
		}
		else if (c2 == 1)
		{
			int i = 0;
			for (i = 0; i < c1 - 1; i++)
			{
				ib->addIndex(vId + i);
				ib->addIndex(vId + c1);
				ib->addIndex(vId + i + 1);
			}

			ib->addIndex(vId + i);
			ib->addIndex(vId + c1);
			ib->addIndex(vId);
		}
		else
		{
			int i = 0;
			for (i = 0; i < c1 - 1; i++)
			{
				ib->addIndex(vId + i);
				ib->addIndex(vId + c1 + i);
				ib->addIndex(vId + c1 + i + 1);

				ib->addIndex(vId + i);
				ib->addIndex(vId + c1 + i + 1);
				ib->addIndex(vId + i + 1);
			}

			ib->addIndex(vId + i);
			ib->addIndex(vId + i + c1);
			ib->addIndex(vId + c1);

			ib->addIndex(vId + i);
			ib->addIndex(vId + c1);
			ib->addIndex(vId);
		}

		vId += c1;
	}

	m_mesh->addMeshBuffer(meshBuffer, material->getName(), material);

	// recalc bbox for culling
	m_mesh->recalculateBoundingBox();

	// remeber set static mesh buffer to optimize (it will stored on GPU)
	m_mesh->setHardwareMappingHint(EHM_STATIC);

	meshBuffer->drop();
}

void CLatheMesh::drawOutline()
{
	if (m_mesh != NULL)
	{
		CSceneDebug* sceneDebug = CSceneDebug::getInstance();

		IMeshBuffer* meshBuffer = m_mesh->getMeshBuffer(0);
		IVertexBuffer* vb = meshBuffer->getVertexBuffer();

		S3DVertex* vtx = (S3DVertex*)vb->getVertices();

		u32 numPoint = m_outline.Points.size();
		u32 vId = 0;

		SColor color(255, 255, 0, 0);

		for (u32 j = 0; j < numPoint - 1; j++)
		{
			int c1 = m_outline.VertexCount[j];
			int c2 = m_outline.VertexCount[j + 1];

			if (c1 == 1)
			{
				for (int i = 0; i < c2; i++)
				{
					sceneDebug->addLine(
						vtx[vId].Pos,
						vtx[vId + c1 + i].Pos,
						color
					);
				}
			}
			else if (c2 == 1)
			{
				for (int i = 0; i < c1; i++)
				{
					sceneDebug->addLine(
						vtx[vId + i].Pos,
						vtx[vId + c1].Pos,
						color
					);
				}
			}
			else
			{
				for (int i = 0; i < c1; i++)
				{
					sceneDebug->addLine(
						vtx[vId + i].Pos,
						vtx[vId + c1 + i].Pos,
						color
					);

					if (i < c1 - 1)
					{
						sceneDebug->addLine(
							vtx[vId + c1 + i].Pos,
							vtx[vId + c1 + i + 1].Pos,
							color
						);
					}
					else
					{
						sceneDebug->addLine(
							vtx[vId + c1 + i].Pos,
							vtx[vId + c1].Pos,
							color
						);
					}
				}
			}

			vId += c1;
		}
	}
}