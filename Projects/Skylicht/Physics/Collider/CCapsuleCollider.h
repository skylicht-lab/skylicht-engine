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
		 * @brief A capsule collision shape.
		 * @ingroup Physics
		 * 
		 * Example:
		 * @code
		 * Physics::CCapsuleCollider* capsule = gameObject->addComponent<Physics::CCapsuleCollider>();
		 * capsule->setCapsule(0.5f, 2.0f);
		 * @endcode
		 */
		class CCapsuleCollider : public CCollider
		{
		protected:
			float m_radius;
			float m_height;

		public:
			CCapsuleCollider();

			virtual ~CCapsuleCollider();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Gets the radius of the capsule.
			 * @return Radius value.
			 */
			inline float getRadius()
			{
				return m_radius;
			}

			/**
			 * @brief Gets the height of the capsule.
			 * @return Height value.
			 */
			inline float getHeight()
			{
				return m_height;
			}

			/**
			 * @brief Sets the capsule dimensions.
			 * @param radius Radius value.
			 * @param height Height value.
			 */
			inline void setCapsule(float radius, float height)
			{
				m_radius = radius;
				m_height = height;
			}

#ifdef USE_BULLET_PHYSIC_ENGINE
			virtual btCollisionShape* initCollisionShape();
#endif

			DECLARE_GETTYPENAME(CCapsuleCollider)
		};
	}
}