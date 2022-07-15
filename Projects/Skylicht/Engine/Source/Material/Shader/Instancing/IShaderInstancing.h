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

#include "Utils/CGameSingleton.h"
#include "Transform/CWorldTransformData.h"
#include "IndirectLighting/CIndirectLightingData.h"

namespace Skylicht
{
	class CMaterial;

	class IShaderInstancing
	{
	protected:
		video::IVertexDescriptor* m_baseVtxDescriptor;
		video::IVertexDescriptor* m_vtxDescriptor;

	public:
		IShaderInstancing();

		virtual ~IShaderInstancing();

		virtual bool isSupport(IMeshBuffer* mb);

		virtual bool isSupport(IMesh* mesh);

		virtual IMeshBuffer* copyConvertMeshBuffer(IMeshBuffer* mb);

		virtual IVertexBuffer* createInstancingMeshBuffer() = 0;

		virtual IMeshBuffer* createMeshBuffer(video::E_INDEX_TYPE type) = 0;

		virtual bool applyInstancing(IMeshBuffer* meshbuffer, IVertexBuffer* instancingBuffer);

		virtual bool removeInstancing(IMeshBuffer* meshbuffer);

		virtual bool applyInstancing(IMesh* mesh, IVertexBuffer* instancingBuffer);

		virtual bool removeInstancing(IMesh* mesh);

		virtual void batchIntancing(IVertexBuffer* vtxBuffer,
			core::array<CMaterial*>& materials,
			core::array<CWorldTransformData*>& worlds,
			core::array<CIndirectLightingData*> lightings) = 0;

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