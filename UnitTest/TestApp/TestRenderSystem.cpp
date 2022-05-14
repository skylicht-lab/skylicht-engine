#include "pch.h"
#include "Base.hh"
#include "TestRenderSystem.h"

bool g_passEntityDataDestructor = false;

IMPLEMENT_DATA_TYPE_INDEX(CTestEntityData)

CTestEntityData::CTestEntityData()
	:TestMesh(NULL)
{

}

CTestEntityData::~CTestEntityData()
{
	g_passEntityDataDestructor = true;

	if (TestMesh != NULL)
	{
		TestMesh->drop();
	}
}

void CTestEntityData::initCubeMesh(float size)
{
	ISceneManager* sceneManager = getIrrlichtDevice()->getSceneManager();
	TestMesh = sceneManager->getGeometryCreator()->createCubeMesh(core::vector3df(size));
}

ACTIVATOR_REGISTER(CTestEntityData);

bool g_passSystemInit = false;
bool g_passSystemUpdate = false;
bool g_passSystemBeginQuery = false;
bool g_passSystemOnQuery = false;
bool g_passSystemRender = false;

CTestRenderSystem::CTestRenderSystem()
{

}

CTestRenderSystem::~CTestRenderSystem()
{

}

void CTestRenderSystem::beginQuery(CEntityManager* entityManager)
{
	m_testEntities.set_used(0);
	m_testTransforms.set_used(0);

	g_passSystemBeginQuery = true;
}

void CTestRenderSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
{
	CTestEntityData* testEntity = entity->getData<CTestEntityData>();
	if (testEntity != NULL)
	{
		CWorldTransformData* testTransform = dynamic_cast<CWorldTransformData*>(entity->getDataByIndex(CWorldTransformData::DataTypeIndex));

		if (testEntity != NULL && testTransform != NULL)
		{
			m_testEntities.push_back(testEntity);
			m_testTransforms.push_back(testTransform);

			g_passSystemOnQuery = true;
		}
	}
}

void CTestRenderSystem::init(CEntityManager* entityManager)
{
	g_passSystemInit = true;
}

void CTestRenderSystem::update(CEntityManager* entityManager)
{
	g_passSystemUpdate = true;
}

void CTestRenderSystem::render(CEntityManager* entityManager)
{
	IVideoDriver* driver = getVideoDriver();

	for (int i = 0, n = (int)m_testEntities.size(); i < n; i++)
	{
		driver->setTransform(video::ETS_WORLD, m_testTransforms[i]->World);

		IMesh* mesh = m_testEntities[i]->TestMesh;
		if (mesh == NULL)
			continue;

		int numMeshBuffer = mesh->getMeshBufferCount();

		for (int j = 0; j < numMeshBuffer; j++)
		{
			driver->setMaterial(mesh->getMeshBuffer(j)->getMaterial());
			driver->drawMeshBuffer(mesh->getMeshBuffer(j));
		}

		g_passSystemRender = (numMeshBuffer > 0);
	}
}

bool isTestRenderSystemPass()
{
	return g_passSystemInit &&
		g_passSystemUpdate &&
		g_passSystemBeginQuery &&
		g_passSystemOnQuery &&
		g_passSystemRender &&
		g_passEntityDataDestructor;
}