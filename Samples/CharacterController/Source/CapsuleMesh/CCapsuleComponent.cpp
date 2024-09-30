#include "pch.h"
#include "SkylichtEngine.h"
#include "CCapsuleComponent.h"
#include "CCapsuleMesh.h"

CCapsuleComponent::CCapsuleComponent()
{
	m_material = new CMaterial("default_material", "BuiltIn/Shader/Basic/VertexColor.xml");
}

CCapsuleComponent::~CCapsuleComponent()
{

}

void CCapsuleComponent::initComponent()
{

}

void CCapsuleComponent::updateComponent()
{

}

void CCapsuleComponent::init(float radius, float height)
{
	CEntity* entity = m_gameObject->getEntity();

	// init capsule mesh
	CCapsuleMesh capsuleMesh;
	capsuleMesh.init(radius, height, m_material);

	// add cube mesh data to entity
	CRenderMeshData* renderMesh = entity->getData<CRenderMeshData>();
	if (renderMesh == NULL)
		renderMesh = entity->addData<CRenderMeshData>();

	renderMesh->setShareMesh(capsuleMesh.getMesh());

	// add culling
	CCullingData* culling = entity->getData<CCullingData>();
	if (culling == NULL)
		culling = entity->addData<CCullingData>();
	culling->Type = CCullingData::BoundingBox;
}