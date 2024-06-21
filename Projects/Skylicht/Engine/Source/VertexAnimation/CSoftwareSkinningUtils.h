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

#include "RenderMesh/CRenderMesh.h"
#include "Animation/Skeleton/CSkeleton.h"

namespace Skylicht
{
	class SKYLICHT_API CSoftwareSkinningUtils
	{
	public:
		static CMesh* initSoftwareSkinning(CMesh* originalMesh);

		static CMesh* initSoftwareBlendShape(CMesh* originalMesh);

		static void resetVertexID(CMesh* mesh);

		static void softwareSkinning(CMesh* renderMesh, CSkinnedMesh* originalMesh, CSkinnedMesh* blendShapeMesh);

		static void softwareSkinningTangent(CMesh* renderMesh, CSkinnedMesh* originalMesh, CSkinnedMesh* blendShapeMesh);

		static void skinVertex(CSkinnedMesh::SJoint* arrayJoint,
			core::vector3df& vertex,
			core::vector3df& normal,
			const core::vector3df& srcPos,
			const core::vector3df& srcNormal,
			const float* boneID,
			const float* boneWeight,
			int boneIndex);

		static void softwareBlendShape(CMesh* blendShape, CMesh* originalMesh);
	};
}