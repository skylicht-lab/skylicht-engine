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

#include "CTransformEuler.h"

namespace Skylicht
{
	/// @brief This is an object class that describes a transform, but the input data is a matrix.
	/// @ingroup Transform
	/// 
	/// In some cases, when using third-party modules, converting a transform back to its position or rotation can have performance limitations. Therefore, it's better to use the matrix directly.
	/// For example, from the Bullet3 physics engine.
	/// And GameObjects with an attached Physics::CRigidbody component will have their transform converted to a matrix.
	///
	/// Although CTransformMatrix inherits from CTransformEuler, but some of the inherits member functions will not work like setPosition, setRotation, setScale...
	/// 
	/// You can call the setupMatrixTransform function from CGameObject to convert the transform from Euler to a Matrix.
	/// 
	/// Example code:
	/// @code
	/// CGameObject* gameObject = zone->createEmptyObject();
	/// gameObject->setupMatrixTransform();
	/// CTransformMatrix* transform = gameObject->getTransformMatrix();
	/// core::matrix4 mat;
	/// mat.setTranslation(core::vector3df(-1.0f, 1.0f, 0.0f));
	/// transform->setRelativeTransform(mat);
	/// @endcode
	class SKYLICHT_API CTransformMatrix : public CTransformEuler
	{
	protected:

	public:
		CTransformMatrix();

		virtual ~CTransformMatrix();

		virtual void reset();

		virtual void initComponent();

		virtual void updateComponent();

	public:

		virtual core::vector3df getRelativePosition()
		{
			return m_transform.getTranslation();
		}

		inline core::vector3df getPosition()
		{
			return m_transform.getTranslation();
		}

		inline core::vector3df getRotation()
		{
			return m_transform.getRotationDegrees();
		}

		inline core::vector3df getScale()
		{
			return m_transform.getScale();
		}

		virtual void setRelativeTransform(const core::matrix4& mat);

		virtual const core::matrix4& getRelativeTransform();

		virtual void getRelativeTransform(core::matrix4& matrix);

		virtual CObjectSerializable* createSerializable();

		DECLARE_GETTYPENAME(CTransformMatrix)
	};
}