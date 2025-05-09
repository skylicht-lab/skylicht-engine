/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "Instancing/CInstancingMaterialData.h"

namespace Skylicht
{
	class SKYLICHT_API CRenderMeshInstancing : public CEntityHandler
	{
	protected:
		CEntity* m_root;
		core::array<CEntity*> m_baseEntities;

		std::vector<CWorldTransformData*> m_renderTransforms;
		std::vector<CWorldTransformData*> m_transforms;
		std::vector<CRenderMeshData*> m_renderers;

		ArrayMaterial m_materials;

		std::string m_meshFile;
		std::string m_materialFile;

		bool m_loadTexcoord2;
		bool m_loadNormal;
		bool m_fixInverseNormal;

		IVertexBuffer* m_instancingMaterials;
		IVertexBuffer* m_instancingTransform;
		IVertexBuffer* m_instancingLighting;
		int m_shareDataTransform;
		int m_shareDataMaterials;

	public:
		CRenderMeshInstancing();

		virtual ~CRenderMeshInstancing();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		virtual void refreshModelAndMaterial();

		virtual void initFromPrefab(CEntityPrefab* prefab);

		virtual CEntity* spawn();

		virtual CEntity* spawn(const core::matrix4& transform);

		const std::string& getMeshResource()
		{
			return m_meshFile;
		}

		const std::string& getMaterialResource()
		{
			return m_materialFile;
		}

		void initFromMeshFile(const char* path, bool loadNormalMap = true, bool loadTexcoord2 = false);

		void initMaterialFromFile(const char* material);

		void initMaterial(ArrayMaterial& materials, bool cloneMaterial = false);

		void applyShareTransformBuffer();

		void applyShareMaterialBuffer();

		inline int getMaterialCount()
		{
			return (int)m_materials.size();
		}

		CMaterial* getMaterial(int i)
		{
			return m_materials[i];
		}

		core::array<CEntity*>& getBaseEntities()
		{
			return m_baseEntities;
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

		DECLARE_GETTYPENAME(CRenderMeshInstancing)

	protected:

		virtual void releaseMaterial();

		virtual void releaseEntities();

		virtual void releaseBaseEntities();

		void addRendererInstancing(CEntity* entity, CRenderMeshData* baseRenderMesh, CWorldTransformData* baseTransform);

		void addMaterial(CMaterial* material);
	};

}