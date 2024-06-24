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
#include "Instancing/SMeshInstancing.h"
#include "CMesh.h"
#include "CSkinnedMesh.h"

namespace Skylicht
{
	class CEntityPrefab;

	class SKYLICHT_API CRenderMeshData : public IEntityData
	{
	public:
		static void setImportTextureFolder(std::vector<std::string>& folders);

	protected:
		CMesh* RenderMesh;
		CMesh* SoftwareSkinnedMesh;
		CMesh* SoftwareBlendShapeMesh;

		bool IsSkinnedMesh;
		bool IsSoftwareSkinning;
		bool IsSoftwareBlendShape;
		bool IsInstancing;
		bool IsSkinnedInstancing;

		SMeshInstancing* MeshInstancing;

		bool Visible;

	public:

		CRenderMeshData();

		virtual ~CRenderMeshData();

		inline CMesh* getMesh()
		{
			return RenderMesh;
		}

		inline CMesh* getSoftwareSkinnedMesh()
		{
			return SoftwareSkinnedMesh;
		}

		inline CMesh* getSoftwareBlendShapeMesh()
		{
			return SoftwareBlendShapeMesh;
		}

		inline SMeshInstancing* getMeshInstancing()
		{
			return MeshInstancing;
		}

		void setMesh(CMesh* mesh);

		void setShareMesh(CMesh* mesh);

		void initSoftwareBlendShape();

		void initSoftwareSkinning();

		void releaseSoftwareSkinning();

		inline bool isSoftwareSkinning()
		{
			return IsSoftwareSkinning;
		}

		inline bool isInstancing()
		{
			return IsInstancing;
		}

		inline bool isSkinnedInstancing()
		{
			return IsSkinnedInstancing;
		}

		void setSoftwareSkinning(bool b)
		{
			IsSoftwareSkinning = b;
		}

		inline bool isSoftwareBlendShape()
		{
			return IsSoftwareBlendShape;
		}

		inline bool isSkinnedMesh()
		{
			return IsSkinnedMesh;
		}

		inline void setSkinnedMesh(bool b)
		{
			IsSkinnedMesh = b;
		}

		inline void setVisible(bool b)
		{
			Visible = b;
		}

		inline bool isVisible()
		{
			return Visible;
		}

		void setInstancing(bool b);

		void setSkinnedInstancing(bool b);

		bool setMaterial(CMaterial* material);

		bool unusedMaterial(CMaterial* material);

		virtual bool serializable(CMemoryStream* stream);

		virtual bool deserializable(CMemoryStream* stream, int version);

		DECLARE_GETTYPENAME(CRenderMeshData)
	};

	DECLARE_PUBLIC_DATA_TYPE_INDEX(CRenderMeshData);
}