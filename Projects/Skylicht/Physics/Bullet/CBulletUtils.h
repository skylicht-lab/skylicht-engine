#pragma once

#include "pch.h"

#ifdef USE_BULLET_PHYSIC_ENGINE

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Bullet
{
	inline static btVector3 irrVectorToBulletVector(const core::vector3df& toConvert)
	{
		return btVector3(toConvert.X, toConvert.Y, toConvert.Z);
	}

	inline static core::vector3df bulletVectorToIrrVector(const btVector3& toConvert)
	{
		return core::vector3df(toConvert.x(), toConvert.y(), toConvert.z());
	}

	inline static void quaternionToEuler(const btQuaternion& TQuat, btVector3& TEuler)
	{
		btScalar W = TQuat.getW();
		btScalar X = TQuat.getX();
		btScalar Y = TQuat.getY();
		btScalar Z = TQuat.getZ();
		float WSquared = W * W;
		float XSquared = X * X;
		float YSquared = Y * Y;
		float ZSquared = Z * Z;
		TEuler.setX(atan2f(2.0f * (Y * Z + X * W), -XSquared - YSquared + ZSquared + WSquared));
		TEuler.setY(asinf(-2.0f * (X * Z - Y * W)));
		TEuler.setZ(atan2f(2.0f * (X * Y + Z * W), XSquared - YSquared - ZSquared + WSquared));
		TEuler *= core::RADTODEG;
	}

	static core::vector3df quaternionToIrrEuler(const btQuaternion& TQuat)
	{
		btVector3 bulletEuler;
		quaternionToEuler(TQuat, bulletEuler);
		return Bullet::bulletVectorToIrrVector(bulletEuler);
	}

	inline static core::vector3df bulletTransformToIrrRotation(const btTransform& tr)
	{
		core::matrix4 mat;

#ifdef BT_USE_NEON
		float ptr[16] __attribute__((aligned(16)));
		tr.getOpenGLMatrix(ptr);

		float* m = mat.pointer();
		for (int i = 0; i < 16; i++)
		{
			m[i] = (float)ptr[i];
		}
#else
		f32* ptr;
		ptr = mat.pointer();
		tr.getOpenGLMatrix(ptr);
#endif
		return mat.getRotationDegrees();
	}

	inline static void bulletTransformToIrrMatrix(const btTransform& tr, core::matrix4& mat)
	{
#ifdef BT_USE_NEON
		float ptr[16] __attribute__((aligned(16)));
		tr.getOpenGLMatrix(ptr);

		float* m = mat.pointer();
		for (int i = 0; i < 16; i++)
			m[i] = ptr[i];
#else
		f32* ptr;
		ptr = mat.pointer();
		tr.getOpenGLMatrix(ptr);
#endif
	}

	inline static btTransform irrRotationToBulletTransform(const core::vector3df& rotation)
	{
		core::matrix4 mat;
		mat.setRotationDegrees(rotation);
		btTransform tr;
		tr.setFromOpenGLMatrix(mat.pointer());

		return tr;
	}

	inline static btTransform glMatrixToBulletTransform(float* matrix)
	{
		btTransform tr;
		tr.setFromOpenGLMatrix(matrix);

		return tr;
	}
}

#endif