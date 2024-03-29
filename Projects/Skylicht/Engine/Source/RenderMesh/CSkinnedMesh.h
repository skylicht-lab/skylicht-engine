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

#include "CMesh.h"
#include "Entity/IEntityData.h"
#include "RenderMesh/CJointData.h"

#define GPU_BONES_COUNT 64

namespace Skylicht
{
	class SKYLICHT_API CSkinnedMesh : public CMesh
	{
	public:
		struct SJoint
		{
			// bindPoseMatrix = joint.globalInversedMatrix * mesh.BindShapeMatrix (collada loader)
			core::matrix4 BindPoseMatrix;

			// pointer to gpu skinning matrix
			f32* SkinningMatrix;

			// Entity index, that have JointData
			int EntityIndex;

			// Link to joint transform
			CJointData* JointData;

			std::string Name;

			SJoint()
			{
				EntityIndex = -1;
				JointData = NULL;
				SkinningMatrix = NULL;
			}
		};

	public:
		core::array<SJoint> Joints;

		// this matrix will push to GPU
		f32* SkinningMatrix;

	public:
		CSkinnedMesh();

		virtual ~CSkinnedMesh();

		virtual CMesh* clone();
	};
}