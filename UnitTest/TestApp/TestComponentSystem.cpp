#include "Base.hh"
#include "TestComponentSystem.h"
#include "TestRenderSystem.h"
#include "RenderMesh/CRenderMeshData.h"

bool g_initComponentPass = false;
bool g_updateComponentPass = false;
bool g_postUpdateComponentPass = false;
bool g_endUpdateComponentPass = false;
bool g_releaseComponentPass = false;

bool g_deleteEntityDataPass = false;

TestComponent::TestComponent()
{

}

TestComponent::~TestComponent()
{
	g_releaseComponentPass = true;

	if (m_gameObject->getEntity()->removeData<CTestEntityData>() == true)
	{
		g_deleteEntityDataPass = true;
	}
}

void TestComponent::initComponent()
{
	g_initComponentPass = true;

	TEST_CASE("Add entity data");
	TEST_ASSERT_THROW(m_gameObject->getEntity()->addData<CTestEntityData>() != NULL);

	TEST_CASE("Add entity data by activator");
	TEST_ASSERT_THROW(m_gameObject->getEntity()->addDataByActivator("CRenderMeshData") != NULL);
	TEST_ASSERT_THROW(GET_ENTITY_DATA(m_gameObject->getEntity(), CRenderMeshData) != NULL);


	TEST_CASE("Get entity data");
	CTestEntityData* entityData = m_gameObject->getEntity()->getData<CTestEntityData>();
	TEST_ASSERT_THROW(entityData != NULL);
	entityData->initCubeMesh(1.0f);

	entityData = dynamic_cast<CTestEntityData*>(GET_ENTITY_DATA(m_gameObject->getEntity(), CTestEntityData));
	TEST_ASSERT_THROW(entityData != NULL);

	TEST_CASE("Add render system");
	TEST_ASSERT_THROW(m_gameObject->getEntityManager()->addRenderSystem<CTestRenderSystem>() != NULL);
}

void TestComponent::updateComponent()
{
	g_updateComponentPass = true;
}

void TestComponent::postUpdateComponent()
{
	g_postUpdateComponentPass = true;
}

void TestComponent::endUpdate()
{
	g_endUpdateComponentPass = true;
}

void testComponent(CGameObject* obj)
{
	TEST_CASE("Add component system");
	TestComponent* comp = obj->addComponent<TestComponent>();
	TEST_ASSERT_THROW(comp != NULL);
	TEST_ASSERT_THROW(comp->getGameObject() == obj);

	TEST_CASE("Get component system");
	TestComponent* getComp = obj->getComponent<TestComponent>();
	TEST_ASSERT_THROW(getComp != NULL);
	TEST_ASSERT_THROW(getComp == comp);

	TEST_CASE("Remove component system");
	TEST_ASSERT_THROW(obj->removeComponent<TestComponent>() == true);
	TEST_ASSERT_THROW(obj->getComponent<TestComponent>() == NULL);

	// re-add component for update component test
	obj->addComponent<TestComponent>();
}

bool isTestComponentPass()
{
	return g_initComponentPass &&
		g_releaseComponentPass &&
		g_updateComponentPass &&
		g_postUpdateComponentPass &&
		g_endUpdateComponentPass &&
		g_deleteEntityDataPass;
}