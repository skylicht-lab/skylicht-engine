#pragma once

#include "Entity/IEntityData.h"
#include "Entity/IRenderSystem.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformData.h"

class CTestEntityData : public Skylicht::IEntityData
{
public:
	IMesh *TestMesh;

public:
	CTestEntityData();

	virtual ~CTestEntityData();

	void initCubeMesh(float size);
};

class CTestRenderSystem : public Skylicht::IRenderSystem
{
protected:
	core::array<CTestEntityData*> m_testEntities;
	core::array<CWorldTransformData*> m_testTransforms;

public:
	CTestRenderSystem();

	virtual ~CTestRenderSystem();

	virtual void beginQuery();

	virtual void onQuery(CEntityManager *entityManager, CEntity *entity);

	virtual void init(CEntityManager *entityManager);

	virtual void update(CEntityManager *entityManager);

	virtual void render(CEntityManager *entityManager);
};

bool isTestRenderSystemPass();