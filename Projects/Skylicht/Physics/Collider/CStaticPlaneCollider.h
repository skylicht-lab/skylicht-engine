/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#include "CCollider.h"

namespace Skylicht
{
	namespace Physics
	{
		/**
		 * @brief A static infinite plane collision shape.
		 * @ingroup Physics
		 */
		class CStaticPlaneCollider : public CCollider
		{
		protected:
			core::vector3df m_normal;
			float m_d;

		public:
			CStaticPlaneCollider();

			virtual ~CStaticPlaneCollider();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Gets the normal of the plane.
			 * @return Normal vector.
			 */
			const core::vector3df& getNormal()
			{
				return m_normal;
			}

			/**
			 * @brief Gets the plane constant (distance from origin).
			 * @return Constant value D.
			 */
			float getD()
			{
				return m_d;
			}

			/**
			 * @brief Sets the plane using a normal and a constant.
			 * @param normal Normal vector.
			 * @param d Constant D.
			 */
			void setPlane(const core::vector3df& normal, float d);

			/**
			 * @brief Sets the plane using a normal and a point on the plane.
			 * @param normal Normal vector.
			 * @param origin A point on the plane.
			 */
			void setPlane(const core::vector3df& normal, const core::vector3df& origin);

#ifdef USE_BULLET_PHYSIC_ENGINE
			virtual btCollisionShape* initCollisionShape();
#endif

			DECLARE_GETTYPENAME(CStaticPlaneCollider)
		};
	}
}