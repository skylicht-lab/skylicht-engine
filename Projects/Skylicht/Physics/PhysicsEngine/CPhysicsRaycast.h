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

#include "Collider/CCollider.h"
#include "RigidBody/CRigidbody.h"

namespace Skylicht
{
	namespace Physics
	{
		/**
		 * @brief Result structure for a raycast that returns all hits.
		 * @ingroup Physics
		 */
		struct SAllRaycastResult
		{
			core::array<CCollider*> Colliders; ///< Array of colliders hit by the ray.        
			core::array<CRigidbody*> Bodies;   ///< Array of rigid bodies hit by the ray.     

			core::vector3df RayFromWorld; ///< Ray start position in world space.
			core::vector3df RayToWorld;   ///< Ray end position in world space.

			core::array<core::vector3df> HitNormalWorld; ///< Normals at hit points in world space.
			core::array<core::vector3df> HitPointWorld;  ///< Hit positions in world space.   
			core::array<float> HitFractions;             ///< Hit fractions (0 to 1) along the ray.
		};

		/**
		 * @brief Result structure for a raycast that returns only the closest hit.
		 * @ingroup Physics
		 */
		struct SClosestRaycastResult
		{
			CCollider* Collider; ///< Closest collider hit by the ray.
			CRigidbody* Body;     ///< Closest rigid body hit by the ray.

			float ClosestHitFraction; ///< Closest hit fraction (0 to 1) along the ray.

			core::vector3df RayFromWorld;   ///< Ray start position in world space.
			core::vector3df RayToWorld;     ///< Ray end position in world space.
			core::vector3df HitNormalWorld; ///< Normal at the hit point in world space.      
			core::vector3df HitPointWorld;  ///< Hit position in world space.
		};
	}
}
