#pragma once

#include "pch.h"

/**
 * @file CBulletUtils.h
 * @brief Utility functions for converting between Skylicht and Bullet data types.
 */

#ifdef USE_BULLET_PHYSIC_ENGINE

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

/**
 * @brief Namespace containing Bullet-specific utility functions.
 * @ingroup Physics
 */
namespace Bullet
{
	/**
	 * @brief Converts a Skylicht vector to a Bullet vector.
	 * @param toConvert Skylicht vector (core::vector3df).
	 * @return Bullet vector (btVector3).
	 */
	inline static btVector3 irrVectorToBulletVector(const core::vector3df& toConvert)
	{
		return btVector3(toConvert.X, toConvert.Y, toConvert.Z);
	}

	/**
	 * @brief Converts a Bullet vector to a Skylicht vector.
	 * @param toConvert Bullet vector (btVector3).
	 * @return Skylicht vector (core::vector3df).
	 */
	inline static core::vector3df bulletVectorToIrrVector(const btVector3& toConvert)
	{
		return core::vector3df(toConvert.x(), toConvert.y(), toConvert.z());
	}

	/**
	 * @brief Converts a Bullet quaternion to Euler angles (in degrees).
	 * @param TQuat Bullet quaternion.
	 * @param TEuler Vector to store Euler angles (X=Pitch, Y=Yaw, Z=Roll).
	 */
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

	/**
	 * @brief Converts a Bullet quaternion to a Skylicht Euler vector (in degrees).
	 * @param TQuat Bullet quaternion.
	 * @return Skylicht Euler vector.
	 */
	static core::vector3df quaternionToIrrEuler(const btQuaternion& TQuat)
	{
		btVector3 bulletEuler;
		quaternionToEuler(TQuat, bulletEuler);
		return Bullet::bulletVectorToIrrVector(bulletEuler);
	}

	/**
	 * @brief Extracts the rotation in degrees from a Bullet transform.
	 * @param tr Bullet transform.
	 * @return Skylicht rotation vector.
	 */
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

	/**
	 * @brief Converts a Bullet transform to a Skylicht 4x4 matrix.
	 * @param tr Bullet transform.
	 * @param mat Skylicht matrix to store the result.
	 */
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

	/**
	 * @brief Converts a Skylicht rotation vector to a Bullet transform.
	 * @param rotation Euler angles in degrees.
	 * @return Bullet transform.
	 */
	inline static btTransform irrRotationToBulletTransform(const core::vector3df& rotation)
	{
		core::matrix4 mat;
		mat.setRotationDegrees(rotation);
		btTransform tr;
		tr.setFromOpenGLMatrix(mat.pointer());

		return tr;
	}

	/**
	 * @brief Converts an OpenGL-style matrix array to a Bullet transform.
	 * @param matrix Pointer to 16 floats.
	 * @return Bullet transform.
	 */
	inline static btTransform glMatrixToBulletTransform(float* matrix)
	{
		btTransform tr;
		tr.setFromOpenGLMatrix(matrix);

		return tr;
	}
}

#endif