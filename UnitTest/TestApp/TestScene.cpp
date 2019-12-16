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
	     + Camera
		 + ObjectA
		 + TransformB
				+ TransformC
				    + ObjectD
	*/

	TEST_CASE("Create zone");
	m_zone = m_scene->createZone();
	TEST_ASSERT_THROW(m_zone != NULL);	


	TEST_CASE("Create object");
	CGameObject *obj = m_zone->createEmptyObject();
	obj->setName("ObjectA");
	TEST_ASSERT_THROW(obj != NULL);


	TEST_CASE("Create Camera");
	obj = m_zone->createEmptyObject();
	TEST_ASSERT_THROW(obj != NULL);
	obj->setName("Camera");
	m_camera = obj->addComponent<CCamera>();
	TEST_ASSERT_THROW(m_camera != NULL);

	TEST_CASE("Create container");
	CContainerObject *container = m_zone->createContainerObject();
	container->setName("TransformB");
	TEST_ASSERT_THROW(container != NULL);

	TEST_CASE("Create child object in container");
	container = container->createContainerObject();
	container->setName("TransformC");
	TEST_ASSERT_THROW(obj != NULL);	

	TEST_CASE("Create object");
	obj = container->createEmptyObject();
	obj->setName("ObjectD");
	TEST_ASSERT_THROW(obj != NULL);

	testComponent(obj);
	testTransform(obj);

	TEST_CASE("Force update add/remove");
	m_scene->updateAddRemoveObject();
	m_scene->updateIndexSearchObject();


	TEST_CASE("Render Pipleline init");
	m_forwardRP = new CForwardRP();
	m_deferredRP = new CDeferredRP();

	m_forwardRP->initRender(512, 512);
	m_deferredRP->initRender(512, 512);
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
		CGameObject *obj = m_scene->searchObjectInChild("TransformC");
		TEST_ASSERT_THROW(obj != NULL);
		TEST_ASSERT_STRING_EQUAL(obj->getNameA(), "TransformC");
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
		CGameObject *obj = NULL;
		obj = m_scene->searchObjectInChild("TransformC");
		TEST_ASSERT_THROW(obj == NULL);

		obj = m_scene->searchObjectInChild("ObjectD");
		TEST_ASSERT_THROW(obj == NULL);
	}	

	m_testStep++;
}

void TestScene::render()
{
	m_forwardRP->render(m_camera, m_zone->getEntityManager());
	m_deferredRP->render(m_camera, m_zone->getEntityManager());
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

void testSceneRender()
{
	g_testScene->render();
}

bool isTestScenePass()
{
	TEST_CASE("Test component pass");
	TEST_ASSERT_THROW(isTestComponentPass());

	TEST_CASE("Test scene end");
	delete g_testScene;
	return true;
}