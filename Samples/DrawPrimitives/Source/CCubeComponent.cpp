#include "pch.h"
#include "SkylichtEngine.h"
#include "CCubeComponent.h"
#include "CCubeData.h"

CCubeComponent::CCubeComponent()
{

}

CCubeComponent::~CCubeComponent()
{

}

void CCubeComponent::initComponent()
{
	// add cube mesh data to entity
	// see more code init cube mesh from CCubeData
	CCubeData *cube = m_gameObject->getEntity()->addData<CCubeData>();
	cube->initCube(1.0f);

	// add culling
	CCullingData *culling = m_gameObject->getEntity()->addData<CCullingData>();
	culling->Type = CCullingData::BoundingBox;
}

void CCubeComponent::updateComponent()
{

}