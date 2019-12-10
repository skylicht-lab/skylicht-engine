#include "Base.hh"
#include "TestComponentSystem.h"

bool g_initComponentPass = false;
bool g_updateComponentPass = false;
bool g_postUpdateComponentPass = false;
bool g_endUpdateComponentPass = false;
bool g_releaseComponentPass = false;

TestComponent::TestComponent()
{

}

TestComponent::~TestComponent()
{
	g_releaseComponentPass = true;
}

void TestComponent::initComponent()
{
	g_initComponentPass = true;
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

void testComponent(CGameObject *obj)
{
	TEST_CASE("Add component system");
	TestComponent *comp = obj->addComponent<TestComponent>();
	TEST_ASSERT_THROW(comp != NULL);
	TEST_ASSERT_THROW(comp->getGameObject() == obj);
	obj->initComponent();

	TEST_CASE("Get component system");
	TestComponent *getComp = obj->getComponent<TestComponent>();
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
		g_endUpdateComponentPass;
}