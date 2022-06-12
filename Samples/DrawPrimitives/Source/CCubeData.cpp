#include "pch.h"
#include "SkylichtEngine.h"
#include "CCubeData.h"

CCubeData::CCubeData() :
	CubeMeshBuffer(NULL)
{

}

CCubeData::~CCubeData()
{
	if (CubeMeshBuffer != NULL)
		CubeMeshBuffer->drop();
}

void CCubeData::initCube(float size)
{
	if (CubeMeshBuffer != NULL)
		CubeMeshBuffer->drop();

	if (RenderMesh != NULL)
		RenderMesh->drop();

	// that will render from RenderMesh/CMeshRenderer.cpp
	RenderMesh = new CMesh();

	IVideoDriver* driver = getVideoDriver();

	// Irrlicht Engine Mesh Buffer instance
	// http://irrlicht.sourceforge.net/docu/classirr_1_1scene_1_1_c_mesh_buffer.html
	CubeMeshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);
	IIndexBuffer* ib = CubeMeshBuffer->getIndexBuffer();
	IVertexBuffer* vb = CubeMeshBuffer->getVertexBuffer();

	// Create vertices
	video::SColor clr1(255, 255, 0, 0);
	video::SColor clr2(255, 0, 255, 0);
	video::SColor clr3(255, 0, 0, 255);
	video::SColor clr4(255, 255, 255, 255);
	video::SColor clr5(255, 255, 0, 255);
	video::SColor clr6(255, 255, 255, 0);


	vb->reallocate(12);

	video::S3DVertex Vertices[] = {
		// back
		video::S3DVertex(0, 0, 0, 0, 0, -1, clr1, 0, 1),
		video::S3DVertex(1, 0, 0, 0, 0, -1, clr1, 1, 1),
		video::S3DVertex(1, 1, 0, 0, 0, -1, clr1, 1, 0),
		video::S3DVertex(0, 1, 0, 0, 0, -1, clr1, 0, 0),

		// front
		video::S3DVertex(0, 0, 1, 0, 0, 1, clr2, 0, 1),
		video::S3DVertex(1, 0, 1, 0, 0, 1, clr2, 1, 1),
		video::S3DVertex(1, 1, 1, 0, 0, 1, clr2, 1, 0),
		video::S3DVertex(0, 1, 1, 0, 0, 1, clr2, 0, 0),

		// bottom
		video::S3DVertex(0, 0, 0, 0, -1, 0, clr3, 0, 1),
		video::S3DVertex(0, 0, 1, 0, -1, 0, clr3, 1, 1),
		video::S3DVertex(1, 0, 1, 0, -1, 0, clr3, 1, 0),
		video::S3DVertex(1, 0, 0, 0, -1, 0, clr3, 0, 0),

		// top
		video::S3DVertex(0, 1, 0, 0, 1, 0, clr4, 0, 1),
		video::S3DVertex(0, 1, 1, 0, 1, 0, clr4, 1, 1),
		video::S3DVertex(1, 1, 1, 0, 1, 0, clr4, 1, 0),
		video::S3DVertex(1, 1, 0, 0, 1, 0, clr4, 0, 0),

		// left
		video::S3DVertex(1, 0, 0, 1, 0, 0, clr5, 0, 1),
		video::S3DVertex(1, 0, 1, 1, 0, 0, clr5, 1, 1),
		video::S3DVertex(1, 1, 1, 1, 0, 0, clr5, 1, 0),
		video::S3DVertex(1, 1, 0, 1, 0, 0, clr5, 0, 0),

		// right
		video::S3DVertex(0, 0, 0, -1, 0, 0, clr5, 0, 1),
		video::S3DVertex(0, 0, 1, -1, 0, 0, clr5, 1, 1),
		video::S3DVertex(0, 1, 1, -1, 0, 0, clr5, 1, 0),
		video::S3DVertex(0, 1, 0, -1, 0, 0, clr5, 0, 0),
	};

	for (u32 i = 0; i < 24; ++i)
	{
		Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
		Vertices[i].Pos *= size;

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

	// recalc bbox
	CubeMeshBuffer->recalculateBoundingBox();

	// add cube mesh buffer to mesh
	RenderMesh->addMeshBuffer(CubeMeshBuffer, "default_material", NULL);

	// recalc bbox for culling
	RenderMesh->recalculateBoundingBox();

	// remeber set static mesh buffer to optimize (it will stored on GPU)
	RenderMesh->setHardwareMappingHint(EHM_STATIC);
}