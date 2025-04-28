/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CInstancingMaterialData.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CInstancingMaterialData);

	IMPLEMENT_DATA_TYPE_INDEX(CInstancingMaterialData);

	CInstancingMaterialData::CInstancingMaterialData() :
		Enable(false)
	{

	}

	CInstancingMaterialData::~CInstancingMaterialData()
	{
		release();
	}

	void CInstancingMaterialData::release()
	{
		for (u32 i = 0, n = Materials.size(); i < n; i++)
			Materials[i]->drop();

		Materials.clear();
		Enable = false;
	}

	void CInstancingMaterialData::initCustomMaterial(SMeshInstancing* instancing)
	{
		for (u32 i = 0, n = instancing->Materials.size(); i < n; i++)
		{
			CMaterial* srcMaterial = instancing->Materials[i];

			CMaterial* material = new CMaterial(srcMaterial->getName(), srcMaterial->getShaderPath());
			material->copyParams(srcMaterial);

			Materials.push_back(material);
		}
		Enable = true;
	}
}