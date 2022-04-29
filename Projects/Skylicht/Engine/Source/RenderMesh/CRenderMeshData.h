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
#include "Material/CMaterial.h"
#include "CMesh.h"
#include "CSkinnedMesh.h"

namespace Skylicht
{
	class CRenderMeshData : public IEntityData
	{
	public:
		static std::vector<std::string> ImportTextureFolder;

	protected:
		CMesh* RenderMesh;
		CMesh* OriginalMesh;

		bool IsSkinnedMesh;
		bool SoftwareSkinning;

	public:

		CRenderMeshData();

		virtual ~CRenderMeshData();

		inline CMesh* getMesh()
		{
			return RenderMesh;
		}

		inline CMesh* getOriginalMesh()
		{
			return OriginalMesh;
		}

		void setMesh(CMesh* mesh);

		void initSoftwareSkinning();

		inline bool isSoftwareSkinning()
		{
			return SoftwareSkinning;
		}

		void setSoftwareSkinning(bool b)
		{
			SoftwareSkinning = b;
		}

		inline bool isSkinnedMesh()
		{
			return IsSkinnedMesh;
		}

		inline void setSkinnedMesh(bool b)
		{
			IsSkinnedMesh = b;
		}

		void setMaterial(CMaterial* material);

		void unusedMaterial(CMaterial* material);

		virtual bool serializable(CMemoryStream* stream);

		virtual bool deserializable(CMemoryStream* stream);

		DECLARE_GETTYPENAME(CRenderMeshData)
	};
}