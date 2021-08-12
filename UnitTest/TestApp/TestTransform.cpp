#include "Base.hh"
#include "TestTransform.h"

void testTransform(CGameObject *obj)
{
	TEST_CASE("Get object transform");
	CTransformEuler *transform = obj->getTransformEuler();
	TEST_ASSERT_THROW(transform != NULL);
	
	core::vector3df front = transform->getFront();
	TEST_ASSERT_FLOAT_EQUAL(front.X, CTransform::s_oz.X);
	TEST_ASSERT_FLOAT_EQUAL(front.Y, CTransform::s_oz.Y);
	TEST_ASSERT_FLOAT_EQUAL(front.Z, CTransform::s_oz.Z);

	core::vector3df up = transform->getUp();
	TEST_ASSERT_FLOAT_EQUAL(up.X, CTransform::s_oy.X);
	TEST_ASSERT_FLOAT_EQUAL(up.Y, CTransform::s_oy.Y);
	TEST_ASSERT_FLOAT_EQUAL(up.Z, CTransform::s_oy.Z);

	core::vector3df right = transform->getRight();
	TEST_ASSERT_FLOAT_EQUAL(right.X, CTransform::s_ox.X);
	TEST_ASSERT_FLOAT_EQUAL(right.Y, CTransform::s_ox.Y);
	TEST_ASSERT_FLOAT_EQUAL(right.Z, CTransform::s_ox.Z);



	TEST_CASE("Test set transform matrix");
	core::matrix4 m;
	m.setRotationDegrees(core::vector3df(0.0f, 90.0f, 0.0f));
	m.setTranslation(core::vector3df(100, 100, 100));
	transform->setRelativeTransform(m);

	core::vector3df pos = transform->getPosition();
	core::vector3df rot = transform->getRotation();
	TEST_ASSERT_FLOAT_EQUAL(pos.X, 100.0f);
	TEST_ASSERT_FLOAT_EQUAL(pos.Y, 100.0f);
	TEST_ASSERT_FLOAT_EQUAL(pos.Z, 100.0f);
	TEST_ASSERT_FLOAT_EQUAL(rot.Y, 90.0f);
	// front oz is rotate 90 to right
	front = transform->getFront();
	TEST_ASSERT_FLOAT_EQUAL(front.X, CTransform::s_ox.X);
	TEST_ASSERT_FLOAT_EQUAL(front.Y, CTransform::s_ox.Y);
	TEST_ASSERT_FLOAT_EQUAL(front.Z, CTransform::s_ox.Z);


	TEST_CASE("Test get transform matrix");
	// rotate ox 90
	core::quaternion q;
	q.fromAngleAxis(90.0f * core::DEGTORAD, CTransform::s_ox);
	transform->setPosition(core::vector3df(100.0f, 200.0f, 300.0f));
	transform->setRotation(q);
	transform->getRelativeTransform(m);

	pos = m.getTranslation();
	TEST_ASSERT_FLOAT_EQUAL(pos.X, 100.0f);
	TEST_ASSERT_FLOAT_EQUAL(pos.Y, 200.0f);
	TEST_ASSERT_FLOAT_EQUAL(pos.Z, 300.0f);

	rot = m.getRotationDegrees();
	TEST_ASSERT_FLOAT_EQUAL(rot.X, 90.0f);
	TEST_ASSERT_FLOAT_EQUAL(rot.Y, 0.0f);
	TEST_ASSERT_FLOAT_EQUAL(rot.Z, 0.0f);

	// front is rotate (90) become to down vector
	front = transform->getFront();
	TEST_ASSERT_FLOAT_EQUAL(front.X, -CTransform::s_oy.X);
	TEST_ASSERT_FLOAT_EQUAL(front.Y, -CTransform::s_oy.Y);
	TEST_ASSERT_FLOAT_EQUAL(front.Z, -CTransform::s_oy.Z);
}