#include "Base.hh"
#include "TestTransform.h"

void testTransform(CGameObject *obj)
{
	TEST_CASE("Get object transform");
	CTransformEuler *transform = obj->getTransformEuler();
	TEST_ASSERT_THROW(transform != NULL);
	
	core::vector3df front = transform->getFront();
	TEST_ASSERT_FLOAT_EQUAL(front.X, Transform::Oz.X);
	TEST_ASSERT_FLOAT_EQUAL(front.Y, Transform::Oz.Y);
	TEST_ASSERT_FLOAT_EQUAL(front.Z, Transform::Oz.Z);

	core::vector3df up = transform->getUp();
	TEST_ASSERT_FLOAT_EQUAL(up.X, Transform::Oy.X);
	TEST_ASSERT_FLOAT_EQUAL(up.Y, Transform::Oy.Y);
	TEST_ASSERT_FLOAT_EQUAL(up.Z, Transform::Oy.Z);

	core::vector3df right = transform->getRight();
	TEST_ASSERT_FLOAT_EQUAL(right.X, Transform::Ox.X);
	TEST_ASSERT_FLOAT_EQUAL(right.Y, Transform::Ox.Y);
	TEST_ASSERT_FLOAT_EQUAL(right.Z, Transform::Ox.Z);



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
	TEST_ASSERT_FLOAT_EQUAL(front.X, Transform::Ox.X);
	TEST_ASSERT_FLOAT_EQUAL(front.Y, Transform::Ox.Y);
	TEST_ASSERT_FLOAT_EQUAL(front.Z, Transform::Ox.Z);


	TEST_CASE("Test get transform matrix");
	// rotate ox 90
	core::quaternion q;
	q.fromAngleAxis(90.0f * core::DEGTORAD, Transform::Ox);
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
	TEST_ASSERT_FLOAT_EQUAL(front.X, -Transform::Oy.X);
	TEST_ASSERT_FLOAT_EQUAL(front.Y, -Transform::Oy.Y);
	TEST_ASSERT_FLOAT_EQUAL(front.Z, -Transform::Oy.Z);
}