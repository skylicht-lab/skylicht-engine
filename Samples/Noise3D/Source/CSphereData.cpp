#include "pch.h"
#include "SkylichtEngine.h"
#include "CSphereData.h"

CSphereData::CSphereData() :
	SphereMeshBuffer(NULL)
{

}

CSphereData::~CSphereData()
{
	if (SphereMeshBuffer != NULL)
		SphereMeshBuffer->drop();
}

void CSphereData::initSphere(float radius, CMaterial *material)
{
	if (SphereMeshBuffer != NULL)
		SphereMeshBuffer->drop();

	if (RenderMesh != NULL)
		RenderMesh->drop();

	RenderMesh = new CMesh();

	IMesh *mesh = getIrrlichtDevice()->getSceneManager()->getGeometryCreator()->createSphereMesh(radius, 32, 32);

	SphereMeshBuffer = mesh->getMeshBuffer(0);

	// convert tangent for normal map
	CMeshUtils::convertToTangentVertices(SphereMeshBuffer);

	// recalc bbox
	SphereMeshBuffer->recalculateBoundingBox();

	// add cube mesh buffer to mesh
	RenderMesh->addMeshBuffer(SphereMeshBuffer, material->getName(), material);

	// recalc bbox for culling
	RenderMesh->recalculateBoundingBox();

	// remeber set static mesh buffer to optimize (it will stored on GPU)
	RenderMesh->setHardwareMappingHint(EHM_STATIC);

	// hold this pointer: 3 holder (RenderMesh, CSphereData, IMesh);
	SphereMeshBuffer->grab();

	// release memory
	mesh->drop();
}