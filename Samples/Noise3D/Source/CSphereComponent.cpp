#include "pch.h"
#include "SkylichtEngine.h"
#include "CSphereComponent.h"
#include "CSphereData.h"

CSphereComponent::CSphereComponent()
{
	m_material = new CMaterial("default_material", "BuiltIn/Shader/Basic/VertexColor.xml");
}

CSphereComponent::~CSphereComponent()
{
	delete m_material;
}

void CSphereComponent::initComponent()
{
	CEntity* entity = m_gameObject->getEntity();

	// Add CSphereData (that inherit CRenderMeshData)
	CSphereData* sphereData = entity->addData<CSphereData>(CRenderMeshData::DataTypeIndex);
	sphereData->initSphere(1.0f, m_material);

	CCullingData* culling = entity->addData<CCullingData>();
	culling->Type = CCullingData::BoundingBox;
}

void CSphereComponent::updateComponent()
{

}