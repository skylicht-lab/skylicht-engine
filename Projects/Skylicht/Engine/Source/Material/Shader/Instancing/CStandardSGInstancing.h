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

#include "IShaderInstancing.h"

namespace Skylicht
{
	struct SVtxSGInstancing
	{
		SVec4 UVScale;
		SVec4 Color;
		SVec4 SpecGloss;

		bool operator==(const SVtxSGInstancing& other) const
		{
			return false;
		}
	};

	class SKYLICHT_API CStandardSGInstancing : public IShaderInstancing
	{
	public:
		CStandardSGInstancing();

		virtual ~CStandardSGInstancing();

		virtual IVertexBuffer* createInstancingVertexBuffer();

		virtual IMeshBuffer* createMeshBuffer(video::E_INDEX_TYPE type);

		virtual void batchIntancing(IVertexBuffer* vtxBuffer,
			CMaterial** materials,
			CEntity** entities,
			int count);
	};
}