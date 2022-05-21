/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CTransformEuler.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CTransformEuler);

	CTransformEuler::CTransformEuler() :
		m_scale(1.0f, 1.0f, 1.0f),
		m_matrixChanged(true)
	{

	}

	CTransformEuler::~CTransformEuler()
	{

	}

	void CTransformEuler::reset()
	{
		m_scale.set(1.0f, 1.0f, 1.0f);
		m_position.set(0.0f, 0.0f, 0.0f);
		m_rotation.set(0.0f, 0.0f, 0.0f);
		m_transform.makeIdentity();
		m_hasChanged = true;
		m_matrixChanged = true;
	}

	void CTransformEuler::initComponent()
	{
		CTransform::initComponent();
	}

	void CTransformEuler::updateComponent()
	{

	}

	void CTransformEuler::setOrientation(const core::vector3df& front, const core::vector3df& up)
	{
		core::vector3df frontVec = front;
		frontVec.normalize();

		core::vector3df upVec = up;
		upVec.normalize();

		core::vector3df rightVec = upVec.crossProduct(frontVec);
		upVec = frontVec.crossProduct(rightVec);

		core::matrix4 rotationMatrix;
		f32* matData = rotationMatrix.pointer();

		matData[0] = rightVec.X;
		matData[1] = rightVec.Y;
		matData[2] = rightVec.Z;
		matData[3] = 0.0f;

		matData[4] = upVec.X;
		matData[5] = upVec.Y;
		matData[6] = upVec.Z;
		matData[7] = 0.0f;

		matData[8] = frontVec.X;
		matData[9] = frontVec.Y;
		matData[10] = frontVec.Z;
		matData[11] = 0.0f;

		matData[12] = 0.0f;
		matData[13] = 0.0f;
		matData[14] = 0.0f;
		matData[15] = 1.0f;

		m_rotation = rotationMatrix.getRotationDegrees();
		m_hasChanged = true;
		m_matrixChanged = true;
	}

	void CTransformEuler::lookAt(const core::vector3df& position)
	{
		float angle = (float)(core::vector2df(position.X - m_position.X, position.Z - m_position.Z).getAngleTrig());
		m_rotation.set(0.0f, 90 - angle, 0.0f);
		m_hasChanged = true;
		m_matrixChanged = true;
	}

	void CTransformEuler::setRelativeTransform(const core::matrix4& mat)
	{
		core::vector3df front = s_oz;
		core::vector3df up = s_oy;

		mat.rotateVect(front);
		mat.rotateVect(up);

		front.normalize();
		up.normalize();

		core::vector3df right = up.crossProduct(front);
		right.normalize();

		core::matrix4 rotationMatrix;
		f32* matData = rotationMatrix.pointer();
		matData[0] = right.X;
		matData[1] = right.Y;
		matData[2] = right.Z;
		matData[3] = 0.0f;

		matData[4] = up.X;
		matData[5] = up.Y;
		matData[6] = up.Z;
		matData[7] = 0.0f;

		matData[8] = front.X;
		matData[9] = front.Y;
		matData[10] = front.Z;
		matData[11] = 0.0f;

		matData[12] = 0.0f;
		matData[13] = 0.0f;
		matData[14] = 0.0f;
		matData[15] = 1.0f;

		core::quaternion q(rotationMatrix);
		q.toEuler(m_rotation);

		m_rotation = m_rotation * core::RADTODEG;
		m_position = mat.getTranslation();
		m_scale = mat.getScale();

		m_hasChanged = true;

		m_transform = mat;
		m_matrixChanged = false;
	}

	const core::matrix4& CTransformEuler::getRelativeTransform()
	{
		if (m_matrixChanged == true)
		{
			m_matrixChanged = false;
			getRelativeTransform(m_transform);
		}

		return m_transform;
	}

	void CTransformEuler::getRelativeTransform(core::matrix4& matrix)
	{
		matrix.makeIdentity();
		matrix.setRotationDegrees(m_rotation);

		f32* m = matrix.pointer();

		m[0] *= m_scale.X;
		m[1] *= m_scale.X;
		m[2] *= m_scale.X;
		m[3] *= m_scale.X;

		m[4] *= m_scale.Y;
		m[5] *= m_scale.Y;
		m[6] *= m_scale.Y;
		m[7] *= m_scale.Y;

		m[8] *= m_scale.Z;
		m[9] *= m_scale.Z;
		m[10] *= m_scale.Z;
		m[11] *= m_scale.Z;

		m[12] = m_position.X;
		m[13] = m_position.Y;
		m[14] = m_position.Z;
	}

	void CTransformEuler::getRotation(core::quaternion& q)
	{
		q = core::quaternion(m_rotation * core::DEGTORAD);
	}

	core::quaternion CTransformEuler::getRotationQuaternion()
	{
		return core::quaternion(m_rotation * core::DEGTORAD);
	}

	void CTransformEuler::getFront(core::vector3df& front)
	{
		core::matrix4 mat;
		mat.setRotationDegrees(m_rotation);

		front = s_oz;
		mat.rotateVect(front);
	}

	core::vector3df CTransformEuler::getFront()
	{
		core::matrix4 mat;
		mat.setRotationDegrees(m_rotation);

		core::vector3df front = s_oz;
		mat.rotateVect(front);
		return front;
	}

	void CTransformEuler::getUp(core::vector3df& up)
	{
		core::matrix4 mat;
		mat.setRotationDegrees(m_rotation);

		up = s_oy;
		mat.rotateVect(up);
	}

	core::vector3df CTransformEuler::getUp()
	{
		core::matrix4 mat;
		mat.setRotationDegrees(m_rotation);

		core::vector3df up = s_oy;
		mat.rotateVect(up);
		return up;
	}

	void CTransformEuler::getRight(core::vector3df& right)
	{
		core::matrix4 mat;
		mat.setRotationDegrees(m_rotation);

		right = s_ox;
		mat.rotateVect(right);
	}

	core::vector3df CTransformEuler::getRight()
	{
		core::matrix4 mat;
		mat.setRotationDegrees(m_rotation);

		core::vector3df right = s_ox;
		mat.rotateVect(right);
		return right;
	}

	CObjectSerializable* CTransformEuler::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();
		object->autoRelease(new CVector3Property(object, "position", getPosition()));
		object->autoRelease(new CVector3Property(object, "rotation", getRotation()));
		object->autoRelease(new CVector3Property(object, "scale", getScale()));
		return object;
	}

	void CTransformEuler::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);
		setPosition(object->get("position", core::vector3df()));
		setRotation(object->get("rotation", core::vector3df()));
		setScale(object->get("scale", core::vector3df(1.0f, 1.0f, 1.0f)));
	}
}