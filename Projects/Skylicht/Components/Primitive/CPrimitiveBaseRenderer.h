/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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

#include "Entity/IRenderSystem.h"
#include "RenderMesh/CMesh.h"
#include "Material/CMaterial.h"
#include "CPrimiviteData.h"

namespace Skylicht
{
	class COMPONENT_API CPrimitiveBaseRenderer : public IRenderSystem
	{
	protected:
		CMesh* m_mesh[CPrimiviteData::Count];
		CMesh* m_meshTangent[CPrimiviteData::Count];

	public:
		CPrimitiveBaseRenderer();

		virtual ~CPrimitiveBaseRenderer();

		CMesh* initMesh(IMesh* primitiveMesh, bool tangent);

		CMesh* initCube(bool tangent);

		CMesh* initPlane(bool tangent);

		CMesh* initMesh(CPrimiviteData::EPrimitive type, bool tangent);
	};
}