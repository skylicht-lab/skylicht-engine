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

#include "Entity/CEntity.h"
#include "Transform/CWorldTransformData.h"
#include "IndirectLighting/CIndirectLightingData.h"

namespace Skylicht
{
	class CMaterial;

	struct SVtxTransform
	{
		core::matrix4 World;

		bool operator==(const SVtxTransform& other) const
		{
			return false;
		}
	};

	struct SVtxIndirectLighting
	{
		core::vector3df D0;
		core::vector3df D1;
		core::vector3df D2;
		core::vector3df D3;

		bool operator==(const SVtxIndirectLighting& other) const
		{
			return false;
		}
	};

	class SKYLICHT_API IShaderInstancing
	{
	protected:
		video::IVertexDescriptor* m_baseVtxDescriptor;
		video::IVertexDescriptor* m_vtxDescriptor;
		video::IVertexDescriptor* m_vtxDescriptorForRenderLighting;

	public:
		IShaderInstancing();

		virtual ~IShaderInstancing();

		virtual bool isSupport(IMeshBuffer* mb);

		virtual bool isSupport(IMesh* mesh);

		virtual IMeshBuffer* createLinkMeshBuffer(IMeshBuffer* mb);

		virtual IVertexBuffer* createInstancingMeshBuffer() = 0;

		virtual IMeshBuffer* createMeshBuffer(video::E_INDEX_TYPE type) = 0;

		static IVertexBuffer* createTransformMeshBuffer();

		static IVertexBuffer* createIndirectLightingMeshBuffer();

		virtual bool applyInstancing(IMeshBuffer* meshbuffer, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		virtual bool applyInstancingForRenderLighting(IMeshBuffer* meshbuffer, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		virtual bool removeInstancing(IMeshBuffer* meshbuffer);

		virtual bool applyInstancing(IMesh* mesh, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		virtual bool applyInstancingForRenderLighting(IMesh* mesh, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		virtual bool removeInstancing(IMesh* mesh);

		virtual void batchIntancing(IVertexBuffer* vtxBuffer,
			CMaterial** materials,
			CEntity** entities,
			int count) = 0;

		static void batchTransformAndLighting(
			IVertexBuffer* tBuffer,
			IVertexBuffer* lBuffer,
			CEntity** entities,
			int count);

		static void batchTransform(
			IVertexBuffer* tBuffer,
			CEntity** entities,
			int count);

		video::IVertexDescriptor* getBaseVertexDescriptor()
		{
			return m_baseVtxDescriptor;
		}

		video::IVertexDescriptor* getInstancingVertexDescriptor()
		{
			return m_vtxDescriptor;
		}
	};
}