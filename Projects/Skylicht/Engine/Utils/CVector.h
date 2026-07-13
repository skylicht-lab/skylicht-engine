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

namespace Skylicht
{
	/**
	 * @brief Common vector and quaternion math helpers.
	 * @ingroup Utilities
	 */
	class SKYLICHT_API CVector
	{
	public:

		/**
		 * @brief Linearly interpolate between two vectors.
		 * @param a Start vector.
		 * @param b End vector.
		 * @param t Interpolation factor clamped to [0, 1].
		 * @return Interpolated vector.
		 */
		static core::vector3df lerp(const core::vector3df& a, const core::vector3df& b, float t);

		/**
		 * @brief Spherically interpolate between two direction vectors.
		 * @param a Start direction.
		 * @param b End direction.
		 * @param t Interpolation factor.
		 * @return Spherically interpolated vector.
		 */
		static core::vector3df slerp(const core::vector3df& a, const core::vector3df& b, float t);

		/**
		 * @brief Reflect a direction vector around a normal.
		 * @param inDirection Incoming direction.
		 * @param normal Surface normal.
		 * @return Reflected direction.
		 */
		static core::vector3df reflect(const core::vector3df& inDirection, const core::vector3df& normal);

		/**
		 * @brief Calculate the angle between two vectors in radians.
		 * @param a First vector.
		 * @param b Second vector.
		 * @return Angle in radians, or 0 when either vector is too small.
		 */
		static float angle(const core::vector3df& a, const core::vector3df& b);

		/**
		 * @brief Project a vector onto another vector.
		 * @param vector Vector to project.
		 * @param onNormal Target direction.
		 * @return Projection of `vector` on `onNormal`, or zero if `onNormal` is too small.
		 */
		static core::vector3df project(const core::vector3df& vector, const core::vector3df& onNormal);

		/**
		 * @brief Project a vector onto a plane.
		 * @param vector Vector to project.
		 * @param planeNormal Plane normal.
		 * @return Projection of `vector` onto the plane, or zero if the normal is too small.
		 */
		static core::vector3df projectOnPlane(const core::vector3df& vector, const core::vector3df& planeNormal);

		/**
		 * @brief Test whether two vectors are parallel or anti-parallel.
		 * @param a First vector.
		 * @param b Second vector.
		 * @return True when the normalized dot product is approximately +/-1.
		 */
		static bool isParallel(const core::vector3df& a, const core::vector3df& b);

		/**
		 * @brief Convert a quaternion to Euler angles.
		 * @param q Quaternion to convert.
		 * @return Euler angles in radians.
		 */
		static core::vector3df quaternionToEuler(const core::quaternion& q);

		/**
		 * @brief Build a quaternion from three orthogonal basis axes.
		 * @param xAxis Local X axis.
		 * @param yAxis Local Y axis.
		 * @param zAxis Local Z axis.
		 * @return Quaternion representing the basis orientation.
		 */
		static core::quaternion getQuaternionFromAxes(const core::vector3df& xAxis, const core::vector3df& yAxis, const core::vector3df& zAxis);
	};
}
