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

#include "pch.h"
#include "CRenderSkinnedInstancing.h"
#include "CTransformTextureData.h"
#include "MeshManager/CMeshManager.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	CRenderSkinnedInstancing::CRenderSkinnedInstancing() :
		m_root(NULL),
		m_loadTexcoord2(false),
		m_loadNormal(true),
		m_fixInverseNormal(true)
	{

	}

	CRenderSkinnedInstancing::~CRenderSkinnedInstancing()
	{
		releaseMaterial();
		releaseEntities();
	}

	void CRenderSkinnedInstancing::releaseMaterial()
	{
		for (CMaterial* m : m_materials)
		{
			for (CRenderMeshData*& renderer : m_renderers)
			{
				renderer->unusedMaterial(m);
			}

			m->drop();
		}
		m_materials.clear();
	}

	void CRenderSkinnedInstancing::releaseEntities()
	{
		removeAllEntities();

		m_allEntities.clear();
		m_renderers.clear();
		m_entities.clear();
	}

	void CRenderSkinnedInstancing::initComponent()
	{

	}

	void CRenderSkinnedInstancing::updateComponent()
	{

	}

	CObjectSerializable* CRenderSkinnedInstancing::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		object->autoRelease(new CBoolProperty(object, "load normal", m_loadNormal));
		object->autoRelease(new CBoolProperty(object, "inserse normal", m_fixInverseNormal));
		object->autoRelease(new CBoolProperty(object, "load texcoord2", m_loadTexcoord2));

		std::vector<std::string> meshExts = { "dae","obj","smesh" };
		std::vector<std::string> materialExts = { "xml","mat" };

		object->autoRelease(new CFilePathProperty(object, "mesh", m_meshFile.c_str(), meshExts));
		object->autoRelease(new CFilePathProperty(object, "material", m_materialFile.c_str(), materialExts));
		return object;
	}

	void CRenderSkinnedInstancing::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_loadNormal = object->get<bool>("load normal", true);
		m_fixInverseNormal = object->get<bool>("inserse normal", true);
		m_loadTexcoord2 = object->get<bool>("load texcoord2", false);

		std::string meshFile = object->get<std::string>("mesh", "");
		std::string materialFile = object->get<std::string>("material", "");

		if (meshFile != m_meshFile)
		{
			m_meshFile = meshFile;

			CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel(
				meshFile.c_str(),
				"",
				m_loadNormal,
				m_fixInverseNormal,
				m_loadTexcoord2,
				false);

			if (prefab != NULL)
				initFromPrefab(prefab);
		}

		if (!materialFile.empty())
		{
			m_materialFile = materialFile;

			std::vector<std::string> textureFolders;
			ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(
				m_materialFile.c_str(),
				true,
				textureFolders
			);

			if (materials.size() > 0)
				initMaterial(materials);
		}
	}

	void CRenderSkinnedInstancing::initTextureTransform(core::matrix4* transforms, u32 w, u32 h, std::map<std::string, int>& bones)
	{
		// w: num frames
		// h: num bones (all)
		CTextureManager* textureMgr = CTextureManager::getInstance();
		CEntityManager* entityMgr = m_gameObject->getEntityManager();

		for (CRenderMeshData* renderer : m_renderers)
		{
			// get skinned mesh & number of bones
			CSkinnedMesh* skinnedMesh = dynamic_cast<CSkinnedMesh*>(renderer->getMesh());
			u32 jointCount = skinnedMesh->Joints.size();

			core::matrix4* boneMatrix = new core::matrix4[jointCount * w];

			for (u32 i = 0, n = jointCount; i < n; i++)
			{
				CSkinnedMesh::SJoint& joint = skinnedMesh->Joints[i];

				// convert to transform index
				u32 id = bones[joint.Name];

				if (id >= h)
				{
					char log[1024];
					sprintf(log, "[CRenderSkinnedInstancing] initTextureTransform %s out of range: %d", joint.Name.c_str(), id);
					os::Printer::log(log);
				}
				else
				{
					for (u32 j = 0; j < w; j++)
					{
						// calc skinned matrix, that like CSkinnedMeshSystem::update
						boneMatrix[i * w + j].setbyproduct_nocheck(transforms[id * w + j], joint.BindPoseMatrix);
					}
				}
			}

			CEntity* entity = entityMgr->getEntity(renderer->EntityIndex);

			CTransformTextureData* data = entity->addData<CTransformTextureData>();
			data->TransformTexture = textureMgr->createTransformTexture2D("BoneTransformTexture", boneMatrix, w, jointCount);

			delete[]boneMatrix;
		}
	}

	void CRenderSkinnedInstancing::refreshModelAndMaterial()
	{
		CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel(
			m_meshFile.c_str(),
			"",
			m_loadNormal,
			m_fixInverseNormal,
			m_loadTexcoord2,
			false);

		if (prefab != NULL)
			initFromPrefab(prefab);

		if (!m_materialFile.empty())
		{
			std::vector<std::string> textureFolders;
			ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(
				m_materialFile.c_str(),
				true,
				textureFolders
			);

			if (materials.size() > 0)
				initMaterial(materials);
		}
	}

	void CRenderSkinnedInstancing::initFromPrefab(CEntityPrefab* prefab)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldTransformData* rootTransform = GET_ENTITY_DATA(m_root, CWorldTransformData);

		// spawn childs entity
		int numEntities = prefab->getNumEntities();

		core::array<CEntity*> renderEntities;

		// we just add the renderer prefab
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* srcEntity = prefab->getEntity(i);
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				if (srcRender->isSkinnedMesh())
				{
					// just add skinned mesh
					renderEntities.push_back(srcEntity);
				}
			}
		}

		numEntities = renderEntities.size();

		CEntity** entities = entityManager->createEntity(numEntities, m_allEntities);

		// copy entity data
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* spawnEntity = entities[i];
			CEntity* srcEntity = renderEntities[i];

			// copy transform data
			CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();
			if (srcTransform != NULL)
			{
				CWorldTransformData* spawnTransform = spawnEntity->addData<CWorldTransformData>();
				spawnTransform->Name = srcTransform->Name;

				// get the world matrix
				core::matrix4 m = srcTransform->Relative;
				int parentID = srcTransform->ParentIndex;
				while (parentID != -1)
				{
					CEntity* parent = prefab->getEntity(parentID);
					CWorldTransformData* parentTransform = parent->getData<CWorldTransformData>();

					m = parentTransform->Relative * m;
					parentID = parentTransform->ParentIndex;
				}

				spawnTransform->Relative = m;
				spawnTransform->HasChanged = true;
				spawnTransform->Depth = rootTransform->Depth + 1;
				spawnTransform->ParentIndex = m_root->getIndex();

				m_transforms.push_back(spawnTransform);
			}

			// copy render data
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				CRenderMeshData* spawnRender = spawnEntity->addData<CRenderMeshData>();
				spawnRender->setMesh(srcRender->getMesh());
				spawnRender->setSkinnedInstancing(true);

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back(GET_ENTITY_DATA(spawnEntity, CWorldTransformData));
			}
		}

		// for handler on Editor UI
		setEntities(entities, numEntities);
	}

	void CRenderSkinnedInstancing::initFromMeshFile(const char* path)
	{
		m_meshFile = path;
		refreshModelAndMaterial();
	}

	void CRenderSkinnedInstancing::initMaterialFromFile(const char* material)
	{
		m_materialFile = material;
		refreshModelAndMaterial();
	}

	void CRenderSkinnedInstancing::initMaterial(ArrayMaterial& materials, bool cloneMaterial)
	{
		releaseMaterial();

		for (CMaterial* m : materials)
		{
			CMaterial* mat = m;

			if (cloneMaterial)
				mat = m->clone();
			else
				mat->grab();

			for (CRenderMeshData*& renderer : m_renderers)
			{
				renderer->setMaterial(mat);
			}

			m_materials.push_back(mat);
		}

		for (CMaterial* m : m_materials)
			m->applyMaterial();
	}
}