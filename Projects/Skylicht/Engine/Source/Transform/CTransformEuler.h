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

#pragma once

#include "CTransform.h"

namespace Skylicht
{
	class SKYLICHT_API CTransformEuler : public CTransform
	{
	protected:
		core::vector3df m_position;
		core::vector3df m_rotation;
		core::vector3df m_scale;

		core::matrix4	m_transform;

		bool m_matrixChanged;

	public:
		CTransformEuler();

		virtual ~CTransformEuler();

		virtual void reset();

		virtual void initComponent();

		virtual void updateComponent();

	public:

		inline const core::vector3df& getPosition()
		{
			return m_position;
		}

		inline void setPosition(const core::vector3df& pos)
		{
			m_position = pos;
			m_hasChanged = true;
			m_matrixChanged = true;
		}

		inline const core::vector3df& getRotation()
		{
			return m_rotation;
		}

		inline const core::vector3df& getScale()
		{
			return m_scale;
		}

		inline void setScale(const core::vector3df& scale)
		{
			m_scale = scale;
			m_hasChanged = true;
			m_matrixChanged = true;
		}

		inline void setRotation(const core::vector3df& eulerDeg)
		{
			m_rotation = eulerDeg;
			m_hasChanged = true;
			m_matrixChanged = true;
		}

		inline void setRotation(const core::quaternion& q)
		{
			q.toEuler(m_rotation);
			m_rotation *= core::RADTODEG;
			m_hasChanged = true;
			m_matrixChanged = true;
		}

		inline float getYaw()
		{
			return m_rotation.Y;
		}

		inline void setYaw(float deg)
		{
			m_rotation.Y = deg;
			m_hasChanged = true;
			m_matrixChanged = true;
		}

		inline float getPitch()
		{
			return m_rotation.X;
		}

		inline void setPitch(float deg)
		{
			m_rotation.X = deg;
			m_hasChanged = true;
			m_matrixChanged = true;
		}

		inline float getRoll()
		{
			return m_rotation.Z;
		}

		inline void setRoll(float deg)
		{
			m_rotation.Z = deg;
			m_hasChanged = true;
			m_matrixChanged = true;
		}

		void setOrientation(const core::vector3df& front, const core::vector3df& up);

		void lookAt(const core::vector3df& position);

		void getRotation(core::quaternion& q);

		core::quaternion getRotationQuaternion();

		void getFront(core::vector3df& front);

		core::vector3df getFront();

		void getUp(core::vector3df& up);

		core::vector3df getUp();

		void getRight(core::vector3df& right);

		core::vector3df getRight();

		virtual void setRelativeTransform(const core::matrix4& mat);

		virtual const core::matrix4& getRelativeTransform();

		virtual void getRelativeTransform(core::matrix4& matrix);

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CTransformEuler)
	};
}