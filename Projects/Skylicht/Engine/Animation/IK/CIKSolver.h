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

namespace Skylicht
{
	/**
	 * @brief Simple 2-bone Inverse Kinematics (IK) solver.
	 * @ingroup Animation
	 * 
	 * Example: Solving IK for an arm
	 * @code
	 * CIKSolver solver;
	 * core::quaternion qStart, qMid;
	 * 
	 * // poleVector defines the elbow direction
	 * core::vector3df poleVector = elbowPos - shoulderPos; 
	 * 
	 * solver.solveIK(shoulderPos, elbowPos, handPos, targetPos, poleVector, 0.0f, qStart, qMid);
	 * 
	 * // Apply results to bone rotations
	 * shoulderNode->AnimRotation *= qStart;
	 * elbowNode->AnimRotation *= qMid;
	 * @endcode
	 */
	class SKYLICHT_API CIKSolver
	{
	public:
		CIKSolver();

		virtual ~CIKSolver();

		/**
		 * @brief Solves the Inverse Kinematics (IK) for a 2-bone chain (e.g., limb).
		 * 
		 * Given the positions of three joints and a target, it calculates the required rotations
		 * for the start and middle joints to reach the target.
		 * 
		 * @param startJointPos Current world position of the root joint (e.g., Shoulder).
		 * @param midJointPos Current world position of the middle joint (e.g., Elbow).
		 * @param endJointPos Current world position of the effector joint (e.g., Hand).
		 * @param targetPos Target world position for the effector joint.
		 * @param poleVector World direction defining the plane of rotation (where the knee/elbow points).
		 * @param twistValue Additional rotation along the chain direction in degrees.
		 * @param qStart [out] Output rotation quaternion for the start joint.
		 * @param qMid [out] Output rotation quaternion for the middle joint.
		 */
		void solveIK(const core::vector3df& startJointPos,
			const core::vector3df& midJointPos,
			const core::vector3df& endJointPos,
			const core::vector3df& targetPos,
			const core::vector3df& poleVector,
			float twistValue,
			core::quaternion& qStart,
			core::quaternion& qMid);
	};
}