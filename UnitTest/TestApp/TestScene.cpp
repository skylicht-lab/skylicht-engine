#include "Base.hh"
#include "TestScene.h"
#include "TestComponentSystem.h"
#include "TestTransform.h"

#include "Scene/CScene.h"

TestScene::TestScene() : m_testStep(0)
{
	m_scene = new CScene();

	/*
	[Scene Hierarchy]
	SCENE
	  + Zone
		 + ObjectA
		 + TransformB
				+ ObjectC
	*/

	TEST_CASE("Create zone");
	m_zone = m_scene->createZone();
	TEST_ASSERT_THROW(m_zone != NULL);


	TEST_CASE("Create object");
	CGameObject *obj = m_zone->createEmptyObject();
	obj->setName("ObjectA");
	TEST_ASSERT_THROW(obj != NULL);


	TEST_CASE("Create container");
	CContainerObject *container = m_zone->createContainerObject();
	container->setName("TransformB");
	TEST_ASSERT_THROW(container != NULL);


	TEST_CASE("Create child object in container");
	obj = container->createContainerObject();
	obj->setName("ObjectC");
	TEST_ASSERT_THROW(obj != NULL);	

	testComponent(obj);
	testTransform(obj);

	TEST_CASE("Force update add/remove");
	m_scene->updateAddRemoveObject();
	m_scene->updateIndexSearchObject();
}

TestScene::~TestScene()
{
	delete m_scene;
}

void TestScene::update()
{
	m_scene->update();

	if (m_testStep == 0)
	{
		TEST_CASE("Search object");
		CGameObject *obj = m_scene->searchObjectInChild("ObjectC");
		TEST_ASSERT_THROW(obj != NULL);
		TEST_ASSERT_STRING_EQUAL(obj->getNameA(), "ObjectC");
	}
	else if (m_testStep == 1)
	{
		TEST_CASE("Search object");
		CGameObject *obj = m_scene->searchObjectInChild("TransformB");
		TEST_ASSERT_THROW(obj != NULL);
		TEST_ASSERT_STRING_EQUAL(obj->getNameA(), "TransformB");

		// test remove object
		// this will destroy at next update
		obj->remove();
	}
	else if (m_testStep == 2)
	{
		TEST_CASE("Remove object");
		// this object is removed because parent is removed at step 1
		CGameObject *obj = m_scene->searchObjectInChild("ObjectC");
		TEST_ASSERT_THROW(obj == NULL);
	}	

	m_testStep++;
}

TestScene* g_testScene = NULL;

void testScene()
{
	TEST_CASE("Test scene start");
	g_testScene = new TestScene();
}

void testSceneUpdate()
{
	g_testScene->update();
}

bool isTestScenePass()
{
	TEST_CASE("Test component pass");
	TEST_ASSERT_THROW(isTestComponentPass())

	TEST_CASE("Test scene end");
	delete g_testScene;
	return true;
}