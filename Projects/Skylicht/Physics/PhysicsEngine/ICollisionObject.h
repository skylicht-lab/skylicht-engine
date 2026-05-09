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

#include "Components/CComponentSystem.h"

namespace Skylicht
{
	namespace Physics
	{
		/**
		 * @brief Data structure containing information about a collision contact point.
		 * @ingroup Physics
		 */
		struct SCollisionContactPoint
		{
			core::vector3df PositionWorldOnA; ///< Contact point on object A in world space.  
			core::vector3df PositionWorldOnB; ///< Contact point on object B in world space.  
			core::vector3df NormalWorldOnB;   ///< Normal vector on object B at the contact point.
		};

		/**
		 * @brief Base interface for all objects that can participate in collisions.
		 * @ingroup Physics
		 *
		 * This includes rigid bodies and character controllers. It provides common
		 * properties like collision groups, filters, and collision callbacks.
		 * 
		 * Example: Handling collisions
		 * @code
		 * Physics::CRigidbody* body = gameObject->getComponent<Physics::CRigidbody>();
		 * body->OnCollision = [](Physics::ICollisionObject* a, Physics::ICollisionObject* b, Physics::SCollisionContactPoint* points, int numPoints) {
		 *     os::Printer::log("Collision detected!");
		 *     
		 *     // Identify the other object
		 *     // Physics::ICollisionObject* other = (a == body) ? b : a;
		 * };
		 * @endcode
		 */
		class ICollisionObject : public CComponentSystem
		{
		public:
			/**
			 * @brief Types of collision objects.
			 * @ingroup Physics
			 */
			enum ECollisionType
			{				Unknown,
				RigidBody,
				Character
			};
		protected:
			ECollisionType m_collisionType;

			int m_group;
			int m_filter;

		public:
			ICollisionObject();

			virtual ~ICollisionObject();

			/**
			 * @brief Sets the collision group and filter mask.
			 * @param group Bitmask representing the collision group.
			 * @param filter Bitmask representing which groups this object should collide with.
			 */
			virtual void setCollisionGroupAndFilter(int group, int filter);

			/**
			 * @brief Callback function triggered when a collision occurs.
			 * 
			 * Arguments: Object A, Object B, Contact points array, number of contact points.
			 */
			std::function<void(ICollisionObject*, ICollisionObject*, SCollisionContactPoint*, int)> OnCollision;

			/**
			 * @brief Gets the collision type.
			 * @return ECollisionType value.
			 */
			inline ECollisionType getCollisionType()
			{
				return m_collisionType;
			}

			/**
			 * @brief Gets the collision group bitmask.
			 * @return Group bitmask.
			 */
			inline int getCollisionGroup()
			{
				return m_group;
			}

			/**
			 * @brief Gets the collision filter bitmask.
			 * @return Filter bitmask.
			 */
			inline int getCollisionFilter()
			{
				return m_filter;
			}
		};
	}
}