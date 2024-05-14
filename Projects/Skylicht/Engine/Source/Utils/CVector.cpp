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

#include "pch.h"
#include "CVector.h"

// reference from Unity Engine
// https://github.com/Unity-Technologies/UnityCsReference/blob/master/Runtime/Export/Math/Vector3.cs

namespace Skylicht
{
	core::vector3df CVector::lerp(const core::vector3df& a, const core::vector3df& b, float t)
	{
		t = core::clamp(t, 0.0f, 1.0f);

		return core::vector3df(
			a.X + (b.X - a.X) * t,
			a.Y + (b.Y - a.Y) * t,
			a.Z + (b.Z - a.Z) * t
		);
	}

	core::vector3df CVector::slerp(const core::vector3df& a, const core::vector3df& b, float t)
	{
		// https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/
		// Dot product - the cosine of the angle between 2 vectors.
		float dot = a.dotProduct(b);

		// Clamp it to be in the range of Acos()
		// This may be unnecessary, but floating point
		// precision can be a fickle mistress.
		dot = core::clamp(dot, -1.0f, 1.0f);

		// Acos(dot) returns the angle between start and end,
		// And multiplying that by percent returns the angle between
		// start and the final result.
		float theta = acosf(dot) * t;
		core::vector3df relativeVec = b - a * dot;
		relativeVec.normalize();

		// Orthonormal basis
		// The final result.
		core::vector3df ret = (a * cosf(theta)) + (relativeVec * sinf(theta));
		return ret;
	}

	core::vector3df CVector::reflect(const core::vector3df& inDirection, const core::vector3df& normal)
	{
		float factor = -2.0f * normal.dotProduct(inDirection);
		return core::vector3df(
			factor * normal.X + inDirection.X,
			factor * normal.Y + inDirection.Y,
			factor * normal.Z + inDirection.Z);
	}

	float CVector::angle(const core::vector3df& a, const core::vector3df& b)
	{
		float denominator = (float)sqrtf(a.getLengthSQ() * b.getLengthSQ());
		if (denominator < FLT_EPSILON)
			return 0.0f;

		float dot = a.dotProduct(b) / denominator;
		return ((float)acosf(dot));
	}

	core::vector3df CVector::project(const core::vector3df& vector, const core::vector3df& onNormal)
	{
		float sqrMag = onNormal.dotProduct(onNormal);
		if (sqrMag < FLT_EPSILON)
			return core::vector3df();
		else
		{
			float dot = vector.dotProduct(onNormal);
			return core::vector3df(onNormal.X * dot / sqrMag,
				onNormal.Y * dot / sqrMag,
				onNormal.Z * dot / sqrMag);
		}
	}

	core::vector3df CVector::projectOnPlane(const core::vector3df& vector, const core::vector3df& planeNormal)
	{
		float sqrMag = planeNormal.dotProduct(planeNormal);
		if (sqrMag < FLT_EPSILON)
			return core::vector3df();
		else
		{
			float dot = vector.dotProduct(planeNormal);
			return core::vector3df(vector.X - planeNormal.X * dot / sqrMag,
				vector.Y - planeNormal.Y * dot / sqrMag,
				vector.Z - planeNormal.Z * dot / sqrMag);
		}
	}

#define kEpsilon 1.0e-5f
#define equivalent(x, y, fudge) ((x > y) ? (x - y <= fudge) : (y - x <= fudge))

	bool CVector::isParallel(const core::vector3df& v1, const core::vector3df& v2)
	{
		core::vector3df a, b;
		a = v1;
		a.normalize();

		b = v2;
		b.normalize();

		float dotPrd = fabs(a.dotProduct(b));
		return equivalent(dotPrd, 1.0f, kEpsilon);
	}
}