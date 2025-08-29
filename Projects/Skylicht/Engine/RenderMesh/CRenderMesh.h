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
	/// @brief The object class helps to initialize a model from a path or prefab, and gets the model's material to draw it on the screen.
	/// @ingroup RenderMesh
	/// 
	/// @image html RenderMesh/render-mesh-property.jpg width=1200px
	/// 
	/// CRenderMesh can handle both static and skinned meshes. To ensure it renders properly, you need to use the right shader for the specific mesh type. 
	/// You'll need to check if the input model has attributes like normals or texcoord2.
	/// 
	/// CRenderMesh supports instancing multiple objects, but the shader from the material must be linked to an instancing shader. 
	/// If not, the model will not render.
	/// You can find more information in CShader, or in some example shaders in `BuiltIn\Shader\Toon`, `BuiltIn\Shader\SpecularGlossiness\Deferred`
	/// 
	/// You can initialize CRenderMesh using either the Editor or code
	/// @code
	/// std::string folder = "SampleModels/BlendShape/";
	/// std::string modelPath = "SampleModels/BlendShape/Cat.fbx";
	/// std::string materialPath = "SampleModels/BlendShape/Cat.mat";
	/// 
	/// CEntityPrefab* model = CMeshManager::getInstance()->loadModel(modelPath.c_str(), folder.c_str());
	/// if (model)
	/// {
	/// 	CGameObject* renderObj = scene->createEmptyObject();
	/// 
	/// 	CRenderMesh* renderer = renderObj->addComponent<CRenderMesh>();
	/// 	renderer->initFromPrefab(model);
	/// 
	/// 	std::vector<std::string> folders;
	/// 	ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(
	/// 		materialPath.c_str(),
	/// 		true,
	/// 		folders);
	/// 
	/// 	renderer->initMaterial(materials);
	/// 
	/// 	renderObj->addComponent<CIndirectLighting>();
	/// }
	/// @endcode
	/// 
	/// @see CEntityPrefab, CMaterial, CMeshManager, CMaterialManager
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
		bool m_isSortingLights;

		bool m_shadowCasting;

	public:
		CRenderMesh();

		virtual ~CRenderMesh();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

	public:

		void refreshModelAndMaterial(bool reloadModel = true);

		void initFromPrefab(CEntityPrefab* prefab);

		void initFromMeshFile(const char* path, bool loadNormalMap = true, bool loadTexcoord2 = false);

		void initMaterialFromFile(const char* material);

		void changeMaterialFromFile(const char* material);

		void changeShaderForAllMaterials(const char* shader);

		void initMaterial(ArrayMaterial& materials, bool cloneMaterial = false);

		void enableOptimizeForRender(bool b)
		{
			m_optimizeForRender = b;
		}

		void enableInstancing(bool b);

		void enableInstancingMaterialForEntity(bool b);

		void setShadowCasting(bool b);

		inline bool isShadowCasting()
		{
			return m_shadowCasting;
		}

		void setSortLights(bool b);

		inline bool isSortingLights()
		{
			return m_isSortingLights;
		}

		void removeRenderMeshName(const char* name);

		void removeRenderMesh(CRenderMeshData* renderMesh);

		void removeMaterials(ArrayMaterial& materials);

		void attachMeshFromPrefab(CEntityPrefab* prefab, std::vector<std::string>& names, ArrayMaterial& materials, bool cloneMaterial = false, bool resetBoneTransform = false);

		void resetDefaultTransform(CEntityPrefab* prefab, std::vector<std::string>& transformsName);

		inline int getMaterialCount()
		{
			return (int)m_materials.size();
		}

		const std::string& getMeshResource()
		{
			return m_meshFile;
		}

		const std::string& getMaterialResource()
		{
			return m_materialFile;
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

		core::aabbox3df getBounds();

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

		bool addMaterial(CMaterial* material);
	};
}