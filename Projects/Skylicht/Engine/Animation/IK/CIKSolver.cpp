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
#include "CIKSolver.h"
#include "Utils/CVector.h"

#define kPi 3.14159265358979323846264338327950
#define kEpsilon 1.0e-5f
#define absoluteValue(x) ((x) < 0 ? (-(x)) : (x))

namespace Skylicht
{
	CIKSolver::CIKSolver()
	{

	}

	CIKSolver::~CIKSolver()
	{

	}

	void CIKSolver::solveIK(const core::vector3df& startJointPos,
		const core::vector3df& midJointPos,
		const core::vector3df& endJointPos,
		const core::vector3df& targetPos,
		const core::vector3df& poleVector,
		float twistValue,
		core::quaternion& qStart,
		core::quaternion& qMid)
	{
		// vector from startJoint to midJoint
		core::vector3df vector1 = midJointPos - startJointPos;

		// vector from midJoint to effector
		core::vector3df vector2 = endJointPos - midJointPos;

		// vector from startJoint to handle
		core::vector3df vectorH = targetPos - startJointPos;

		// vector from startJoint to effector
		core::vector3df vectorE = endJointPos - startJointPos;

		// lengths of those vectors
		float length1 = vector1.getLength();
		float length2 = vector2.getLength();
		float lengthH = vectorH.getLength();

		// component of the vector1 orthogonal to the vectorE
		core::vector3df vectorO = vector1 - vectorE * ((vector1.dotProduct(vectorE)) / (vectorE.dotProduct(vectorE)));

		//////////////////////////////////////////////////////////////////
		// calculate q12 which solves for the midJoint rotation
		//////////////////////////////////////////////////////////////////

		// angle between vector1 and vector2
		float vectorAngle12 = CVector::angle(vector1, vector2);

		// vector orthogonal to vector1 and 2
		core::vector3df vectorCross12 = vector1.crossProduct(vector2);
		vectorCross12.normalize();

		float lengthHsquared = lengthH * lengthH;

		// angle for arm extension 
		float cos_theta = (lengthHsquared - length1 * length1 - length2 * length2) / (2 * length1 * length2);

		if (cos_theta > 1)
			cos_theta = 1;
		else if (cos_theta < -1)
			cos_theta = -1;

		float theta = acosf(cos_theta);

		// quaternion for arm extension
		core::quaternion q12;
		q12.fromAngleAxis(theta - vectorAngle12, vectorCross12);

		//////////////////////////////////////////////////////////////////
		// calculate qEH which solves for effector rotating onto the handle
		//////////////////////////////////////////////////////////////////
		// vector2 with quaternion q12 applied	
		vector2 = q12 * vector2;

		// vectorE with quaternion q12 applied
		vectorE = vector1 + vector2;

		// quaternion for rotating the effector onto the handle
		core::quaternion qEH;
		qEH.rotationFromTo(vectorE, vectorH);

		//////////////////////////////////////////////////////////////////
		// calculate qNP which solves for the rotate plane
		//////////////////////////////////////////////////////////////////
		// vector1 with quaternion qEH applied
		vector1 = qEH * vector1;

		if (CVector::isParallel(vector1, vectorH))
		{
			// singular case, use orthogonal component instead
			vector1 = qEH * vectorO;
		}

		// quaternion for rotate plane
		core::quaternion qNP;

		core::vector3df vectorHNormal = vectorH;
		vectorHNormal.normalize();

		if (!CVector::isParallel(poleVector, vectorH) && (lengthHsquared != 0))
		{
			// component of vector1 orthogonal to vectorH
			core::vector3df vectorN = vector1 - vectorH * ((vector1.dotProduct(vectorH)) / lengthHsquared);

			// component of pole vector orthogonal to vectorH
			core::vector3df vectorP = poleVector - vectorH * ((poleVector.dotProduct(vectorH)) / lengthHsquared);

			float dotNP = (vectorN.dotProduct(vectorP)) / (vectorN.getLength() * vectorP.getLength());

			if (absoluteValue(dotNP + 1.0) < kEpsilon)
			{
				// singular case, rotate halfway around vectorH
				qNP.fromAngleAxis(kPi, vectorHNormal);
			}
			else
			{
				vector1.normalize();
				vectorH.normalize();

				qNP.rotationFromTo(vectorN, vectorP);
			}
		}

		//////////////////////////////////////////////////////////////////
		// calculate qTwist which adds the twist
		//////////////////////////////////////////////////////////////////
		core::quaternion qTwist;
		qTwist.fromAngleAxis(twistValue * core::DEGTORAD, vectorHNormal);

		// quaternion for the mid joint
		qMid = q12;

		// concatenate the quaternions for the start joint
		qStart = qEH * qNP * qTwist;
	}
}