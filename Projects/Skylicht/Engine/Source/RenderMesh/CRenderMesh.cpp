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

#include "pch.h"
#include "CRenderMesh.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"

#include "Transform/CWorldTransformData.h"
#include "Transform/CWorldInverseTransformSystem.h"
#include "RenderMesh/CRenderMeshData.h"
#include "RenderMesh/CJointData.h"
#include "Culling/CCullingData.h"

#include "MeshManager/CMeshManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CRenderMesh);

	CATEGORY_COMPONENT(CRenderMesh, "Render Mesh", "Renderer");

	CRenderMesh::CRenderMesh() :
		m_loadTexcoord2(false),
		m_loadNormal(true),
		m_fixInverseNormal(true)
	{

	}

	CRenderMesh::~CRenderMesh()
	{
		releaseMaterial();
		releaseEntities();
	}

	void CRenderMesh::releaseMaterial()
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

	void CRenderMesh::releaseEntities()
	{
		if (m_gameObject != NULL)
		{
			CEntityManager* entityManager = m_gameObject->getEntityManager();
			for (u32 i = 0, n = m_allEntities.size(); i < n; i++)
				entityManager->removeEntity(m_allEntities[i]);
		}

		m_allEntities.clear();
		m_renderers.clear();
		m_entities.clear();
	}

	void CRenderMesh::initComponent()
	{

	}

	void CRenderMesh::updateComponent()
	{

	}

	CObjectSerializable* CRenderMesh::createSerializable()
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

	void CRenderMesh::loadSerializable(CObjectSerializable* object)
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

	void CRenderMesh::refreshModelAndMaterial()
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

	void CRenderMesh::initFromPrefab(CEntityPrefab* prefab)
	{
		releaseEntities();

		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldTransformData* rootTransform = (CWorldTransformData*)m_root->getDataByIndex(CWorldTransformData::DataTypeIndex);

		// spawn childs entity
		int numEntities = prefab->getNumEntities();
		CEntity** entities = entityManager->createEntity(numEntities, m_allEntities);

		// map new entity index from src prefab
		std::map<int, int> entityIndex;

		// copy entity data
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* spawnEntity = entities[i];
			CEntity* srcEntity = prefab->getEntity(i);

			// map index
			entityIndex[srcEntity->getIndex()] = spawnEntity->getIndex();

			// copy transform data
			CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();
			if (srcTransform != NULL)
			{
				CWorldTransformData* spawnTransform = spawnEntity->addData<CWorldTransformData>();
				spawnTransform->Name = srcTransform->Name;
				spawnTransform->Relative = srcTransform->Relative;
				spawnTransform->HasChanged = true;
				spawnTransform->Depth = rootTransform->Depth + 1 + srcTransform->Depth;

				if (srcTransform->ParentIndex == -1 || entityIndex.find(srcTransform->ParentIndex) == entityIndex.end())
					spawnTransform->ParentIndex = m_root->getIndex();
				else
					spawnTransform->ParentIndex = entityIndex[srcTransform->ParentIndex];

				m_transforms.push_back(spawnTransform);
			}

			// copy render data
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				CRenderMeshData* spawnRender = spawnEntity->addData<CRenderMeshData>();
				spawnRender->setMesh(srcRender->getMesh());
				spawnRender->setSkinnedMesh(srcRender->isSkinnedMesh());
				spawnRender->setSoftwareSkinning(srcRender->isSoftwareSkinning());

				// init software skinning
				if (spawnRender->isSkinnedMesh() && spawnRender->isSoftwareSkinning() == true)
					spawnRender->initSoftwareSkinning();

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back((CWorldTransformData*)spawnEntity->getDataByIndex(CWorldTransformData::DataTypeIndex));

				// add world inv transform for culling system
				spawnEntity->addData<CWorldInverseTransformData>();
			}

			// copy culling data
			CCullingData* srcCulling = srcEntity->getData<CCullingData>();
			if (srcCulling != NULL)
			{
				CCullingData* spawnCulling = spawnEntity->addData<CCullingData>();
				spawnCulling->Type = srcCulling->Type;
				spawnCulling->Visible = srcCulling->Visible;
			}

			// copy joint data
			CJointData* srcJointData = (CJointData*)srcEntity->getData<CJointData>();
			if (srcJointData != NULL)
			{
				CJointData* spawnJoint = spawnEntity->addData<CJointData>();
				spawnJoint->BoneRoot = srcJointData->BoneRoot;
				spawnJoint->SID = srcJointData->SID;
				spawnJoint->BoneName = srcJointData->BoneName;
				spawnJoint->AnimationMatrix = srcJointData->AnimationMatrix;
				spawnJoint->DefaultAnimationMatrix = srcJointData->DefaultAnimationMatrix;
				spawnJoint->DefaultRelativeMatrix = srcJointData->DefaultRelativeMatrix;
				spawnJoint->RootIndex = m_root->getIndex();
			}
		}

		bool addInvData = false;

		// for handler on Editor UI
		setEntities(entities, numEntities);

		// re-map joint with new entity in CEntityManager
		for (CRenderMeshData*& r : m_renderers)
		{
			if (r->isSkinnedMesh() == true)
			{
				CSkinnedMesh* skinMesh = NULL;

				if (r->isSoftwareSkinning() == true)
					skinMesh = dynamic_cast<CSkinnedMesh*>(r->getOriginalMesh());
				else
					skinMesh = dynamic_cast<CSkinnedMesh*>(r->getMesh());

				if (skinMesh != NULL)
				{
					// alloc animation matrix (Max: 64 bone)
					skinMesh->SkinningMatrix = new f32[16 * GPU_BONES_COUNT];

					for (u32 i = 0, n = skinMesh->Joints.size(); i < n; i++)
					{
						// map entity data to joint
						CSkinnedMesh::SJoint& joint = skinMesh->Joints[i];
						joint.EntityIndex = entityIndex[joint.EntityIndex];
						joint.JointData = (CJointData*)entityManager->getEntity(joint.EntityIndex)->getDataByIndex(CJointData::DataTypeIndex);

						// pointer to skin mesh animation matrix
						joint.SkinningMatrix = skinMesh->SkinningMatrix + i * 16;
					}
				}

				if (addInvData == false)
				{
					if (m_root->getDataByIndex(CWorldInverseTransformData::DataTypeIndex) == NULL)
						m_root->addData<CWorldInverseTransformData>();
					addInvData = true;
				}
			}
		}
	}

	void CRenderMesh::initFromMeshFile(const char* path)
	{
		m_meshFile = path;
		refreshModelAndMaterial();
	}

	void CRenderMesh::initMaterialFromFile(const char* material)
	{
		m_materialFile = material;
		refreshModelAndMaterial();
	}

	void CRenderMesh::initMaterial(ArrayMaterial& materials, bool cloneMaterial)
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