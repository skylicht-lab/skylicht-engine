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

#include "Entity/IEntityData.h"
#include "Material/Shader/Instancing/IShaderInstancing.h"

namespace Skylicht
{
	class CMaterial;

	class SKYLICHT_API CBlendShape : public IReferenceCounted
	{
	public:
		std::string Name;

		float Weight;

		// see function CSoftwareSkinningUtils::softwareBlendShape, CFBXMeshLoader::loadModel
		core::array<u32> VtxId;
		core::array<core::vector3df> Offset;
		core::array<core::vector3df> NormalOffset;

		CBlendShape()
		{
			Weight = 1.0f;
		}

		~CBlendShape()
		{

		}
	};

	class SKYLICHT_API CMesh : public IMesh
	{
	public:
		CMesh();

		virtual ~CMesh();

		virtual CMesh* clone();

		virtual u32 getMeshBufferCount() const;

		virtual IMeshBuffer* getMeshBuffer(u32 nr) const;

		virtual IMeshBuffer* getMeshBuffer(const video::SMaterial& material) const;

		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		core::aabbox3d<f32>* getBoundingBoxPtr();

		virtual void setBoundingBox(const core::aabbox3df& box);

		virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

		virtual void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX);

		virtual void setDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX);

	public:
		void recalculateBoundingBox();

		void addMeshBuffer(IMeshBuffer* buf, const char* materialName = "", CMaterial* m = NULL);

		void addBlendShape(CBlendShape* blendShape);

		void removeAllMeshBuffer();

		void removeMeshBuffer(IMeshBuffer* buf);

		void replaceMeshBuffer(int i, IMeshBuffer* buf);

		IMeshBuffer* getBufferByMaterialID(int materialID);

		CMesh* detachAlphaMeshBuffer();

		int getMeshBufferID(IMeshBuffer* buffer);

		static void applyDoubleSided(IMeshBuffer* buffer);

		core::array<IMeshBuffer*> MeshBuffers;

		core::aabbox3d<f32> BoundingBox;

		std::vector<std::string> MaterialName;

		std::vector<CMaterial*> Materials;

		core::array<CBlendShape*> BlendShape;

		bool UseInstancing;

		CMesh* IndirectLightingMesh;
	};
}