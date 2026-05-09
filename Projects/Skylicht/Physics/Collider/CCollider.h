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

#ifdef USE_BULLET_PHYSIC_ENGINE
#include <btBulletCollisionCommon.h>
#endif

#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	namespace Physics
	{
		/**
		 * @brief Base class for all collision shapes.
		 * @ingroup Physics
		 *
		 * Colliders define the physical shape of a GameObject for collision detection.
		 * They are used by CRigidbody and CCharacterController.
		 */
		class CCollider : public CComponentSystem
		{
		public:
			/**
			 * @brief Supported collider types.
			 * @ingroup Physics
			 */
			enum EColliderType
			{				Box,
				Sphere,
				Plane,
				Cylinder,
				Capsule,
				BvhMesh,
				ConvexMesh,
				Mesh,
				Unknown,
			};

		protected:
			EColliderType m_colliderType;

			core::vector3df m_offset;

			bool m_dynamicSupport;

#ifdef USE_BULLET_PHYSIC_ENGINE
			btCollisionShape* m_shape;
#endif

		public:
			CCollider();

			virtual ~CCollider();

			virtual void initComponent();

			/**
			 * @brief Gets the type of the collider.
			 * @return EColliderType value.
			 */
			EColliderType getColliderType()
			{
				return m_colliderType;
			}

#ifdef USE_BULLET_PHYSIC_ENGINE
			/**
			 * @brief Initializes the Bullet collision shape.
			 * @return Pointer to the initialized btCollisionShape.
			 */
			virtual btCollisionShape* initCollisionShape() = 0;

			/**
			 * @brief Releases the Bullet collision shape.
			 */
			virtual void dropCollisionShape();
#endif

			/**
			 * @brief Triggers the initialization of the attached CRigidbody.
			 */
			void initRigidbody();

			/**
			 * @brief Clamps the size vector to ensure no negative dimensions.
			 * @param size Vector to clamp.
			 */
			void clampSize(core::vector3df& size);

			/**
			 * @brief Checks if this collider type supports dynamic rigid bodies.
			 * @return True if dynamic support is available.
			 */
			inline bool isDynamicSupport()
			{
				return m_dynamicSupport;
			}

			/**
			 * @brief Generates a visual mesh representing the collision shape.
			 * @param maxBBox The maximum bounding box for plane generation.
			 * @return Pointer to the generated CMesh.
			 */
			CMesh* generateMesh(const core::aabbox3df& maxBBox);

			/**
			 * @brief Gets the axis-aligned bounding box of the collider in world space.
			 * @return AABB in world space.
			 */
			core::aabbox3df getBBox();

		protected:

			/**
			 * @brief Helper to generate a mesh from a primitive.
			 */
			CMesh* generateMesh(IMesh* primitive, bool tangent);

			/**
			 * @brief Helper to generate a plane mesh.
			 */
			CMesh* getPlane(const core::plane3df& plane, float sizeX, float sizeZ);

		};
	}
}