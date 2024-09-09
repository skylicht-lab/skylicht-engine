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

#include "irrlicht.h"
using namespace irr::scene;

#include "Material/CMaterial.h"
#include "Material/Shader/Instancing/IShaderInstancing.h"

namespace Skylicht
{
	struct SMeshInstancingGroup;

	// Use in CMeshRendererInstancing
	struct SMeshInstancing
	{
		// base source mesh buffers
		core::array<IMeshBuffer*> MeshBuffers;

		// base source materials
		core::array<CMaterial*> Materials;

		// shader instancing for batching
		core::array<IShaderInstancing*> InstancingShader;

		// shader instancing handle (use on SkinnedInstancing)
		IShaderInstancing* HandleShader;

		// vertex buffer, that batch materials (color, uv, sg, bone..)
		core::array<IVertexBuffer*> MaterialBuffer;

		// vertex buffer, that batch transform
		IVertexBuffer* TransformBuffer;

		// vertex buffer, that bake SH
		IVertexBuffer* IndirectLightingBuffer;

		// mesh instancing, that will render
		IMesh* InstancingMesh;

		// list buffers in InstancingMesh
		core::array<IMeshBuffer*> RenderMeshBuffers;

		// list buffers in IndirectLightingMesh (CMesh property)
		core::array<IMeshBuffer*> RenderLightMeshBuffers;

		// the group, that have many instances will be render
		SMeshInstancingGroup* InstancingGroup;

		// That tell us that, transform and lighting buffer is shared with another Mesh
		bool UseShareTransformBuffer;
		bool UseShareMaterialsBuffer;

		int* ShareDataTransform;
		int* ShareDataMaterials;

		SMeshInstancing()
		{
			HandleShader = NULL;
			InstancingMesh = NULL;
			TransformBuffer = NULL;
			IndirectLightingBuffer = NULL;
			InstancingGroup = NULL;
			UseShareTransformBuffer = false;
			UseShareMaterialsBuffer = false;
			ShareDataTransform = NULL;
			ShareDataMaterials = NULL;
		}

		~SMeshInstancing()
		{
			if (HandleShader)
				delete HandleShader;
		}
	};
}