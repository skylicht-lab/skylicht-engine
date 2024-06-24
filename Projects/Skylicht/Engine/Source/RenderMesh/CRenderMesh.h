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

#include "Entity/CEntityPrefab.h"
#include "Entity/CEntityHandler.h"
#include "Material/CMaterial.h"
#include "Material/CMaterialManager.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	class SKYLICHT_API CRenderMesh : public CEntityHandler
	{
	protected:
		CEntity* m_root;

		std::vector<CWorldTransformData*> m_renderTransforms;
		std::vector<CWorldTransformData*> m_transforms;
		std::vector<CRenderMeshData*> m_renderers;

		ArrayMaterial m_materials;

		std::string m_meshFile;
		std::string m_materialFile;

		bool m_optimizeForRender;
		bool m_loadTexcoord2;
		bool m_loadNormal;
		bool m_fixInverseNormal;
		bool m_enableInstancing;

	public:
		CRenderMesh();

		virtual ~CRenderMesh();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

	public:

		void refreshModelAndMaterial();

		void initFromPrefab(CEntityPrefab* prefab);

		void initFromMeshFile(const char* path);

		void initMaterialFromFile(const char* material);

		void initMaterial(ArrayMaterial& materials, bool cloneMaterial = false);

		void enableOptimizeForRender(bool b)
		{
			m_optimizeForRender = b;
		}

		void enableInstancing(bool b);

		void removeRenderMeshName(const char* name);

		void removeRenderMesh(CRenderMeshData* renderMesh);

		void attachMeshFromPrefab(CEntityPrefab* prefab, std::vector<std::string>& names, ArrayMaterial& materials, bool cloneMaterial = false);

		void resetDefaultTransform(CEntityPrefab* prefab, std::vector<std::string>& transformsName);

		inline int getMaterialCount()
		{
			return (int)m_materials.size();
		}

		CMaterial* getMaterial(int i)
		{
			return m_materials[i];
		}

		std::vector<CRenderMeshData*>& getRenderers()
		{
			return m_renderers;
		}

		std::vector<CWorldTransformData*>& getRenderTransforms()
		{
			return m_renderTransforms;
		}

		std::vector<CWorldTransformData*>& getAllTransforms()
		{
			return m_transforms;
		}

		void printEntites();

		CWorldTransformData* getChildTransform(const char* name);

		int getChildTransforms(const char* name, std::vector<CWorldTransformData*>& childs);

		int getChildTransforms(CWorldTransformData* transform, std::vector<CWorldTransformData*>& childs, size_t from = 0);

		DECLARE_GETTYPENAME(CRenderMesh)

	protected:

		void initNoOptimizeFromPrefab(CEntityPrefab* prefab);

		void initOptimizeFromPrefab(CEntityPrefab* prefab);

		void releaseMaterial();

		void releaseEntities();
	};
}