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
#include "RenderMesh/CRenderMeshData.h"

namespace Skylicht
{
	class SKYLICHT_API CSkinnedInstanceData : public IEntityData
	{
	public:
		int ClipId;
		float Time;
		float TimeFrom;
		float TimeTo;
		int FPS;
		int Frame;
		float Anim2Weight;
		bool Loop;
		bool Pause;

		core::array<CRenderMeshData*> RenderData;
		core::array<ITexture*> TransformTextures;
		core::array<CMaterial*> Materials;
	public:
		CSkinnedInstanceData();

		virtual ~CSkinnedInstanceData();

		DECLARE_GETTYPENAME(CSkinnedInstanceData)
	};

	DECLARE_PUBLIC_DATA_TYPE_INDEX(CSkinnedInstanceData);
}