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

namespace Skylicht
{
	class SKYLICHT_API CCullingData : public IEntityData
	{
	public:
		enum ECulling
		{
			BoundingBox,
			FrustumBox
		};

		core::aabbox3df BBox;

		ECulling Type;

		bool Visible;

		bool CameraCulled;

		u32 CullingLayer;

		bool Occlusion;

	public:
		CCullingData();

		virtual ~CCullingData();

		virtual bool serializable(CMemoryStream* stream);

		virtual bool deserializable(CMemoryStream* stream);

		DECLARE_GETTYPENAME(CCullingData);
	};

	DECLARE_PUBLIC_DATA_TYPE_INDEX(CCullingData);
}