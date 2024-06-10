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
		m_root(NULL),
		m_optimizeForRender(false),
		m_loadTexcoord2(false),
		m_loadNormal(true),
		m_fixInverseNormal(true),
		m_enableInstancing(false)
	{

	}

	CRenderMesh::~CRenderMesh()
	{
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
		releaseMaterial();
		removeAllEntities();

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
		object->autoRelease(new CBoolProperty(object, "optimize", m_optimizeForRender));
		object->autoRelease(new CBoolProperty(object, "instancing", m_enableInstancing));

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

		bool optimize = object->get<bool>("optimize", false);
		bool instancing = object->get<bool>("instancing", false);

		std::string meshFile = object->get<std::string>("mesh", "");
		std::string materialFile = object->get<std::string>("material", "");

		if (meshFile != m_meshFile || optimize != m_optimizeForRender)
		{
			m_meshFile = meshFile;
			m_optimizeForRender = optimize;

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

		// enable instancing
		if (instancing)
			enableInstancing(true);
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

		if (m_optimizeForRender)
			initOptimizeFromPrefab(prefab);
		else
			initNoOptimizeFromPrefab(prefab);
	}

	void CRenderMesh::initNoOptimizeFromPrefab(CEntityPrefab* prefab)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldTransformData* rootTransform = GET_ENTITY_DATA(m_root, CWorldTransformData);

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

				// init software blendshape
				if (srcRender->getMesh()->BlendShape.size() > 0)
					spawnRender->initSoftwareBlendShape();

				// init software skinning
				if (spawnRender->isSkinnedMesh() && spawnRender->isSoftwareSkinning() == true)
					spawnRender->initSoftwareSkinning();

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back(GET_ENTITY_DATA(spawnEntity, CWorldTransformData));

				// add world inv transform for culling system (disable to optimize)
				// spawnEntity->addData<CWorldInverseTransformData>();
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
				spawnJoint->RootIndex = m_root->getIndex();
			}
		}

		bool addInvData = false;

		// for handler on Editor UI
		setEntities(entities, numEntities);

		int boneId = 0;

		// re-map joint with new entity in CEntityManager
		for (CRenderMeshData*& r : m_renderers)
		{
			if (r->isSkinnedMesh() == true)
			{
				CSkinnedMesh* skinMesh = dynamic_cast<CSkinnedMesh*>(r->getMesh());
				if (skinMesh != NULL)
				{
					u32 numJoints = (u32)skinMesh->Joints.size();

					// alloc animation matrix (Max: 64 bone)
					skinMesh->SkinningMatrix = new f32[16 * GPU_BONES_COUNT];

					for (u32 i = 0; i < numJoints; i++)
					{
						// map entity data to joint
						CSkinnedMesh::SJoint& joint = skinMesh->Joints[i];
						joint.EntityIndex = entityIndex[joint.EntityIndex];
						joint.JointData = GET_ENTITY_DATA(entityManager->getEntity(joint.EntityIndex), CJointData);

						// setup bone index for Texture Transform animations
						if (joint.JointData->BoneID == -1)
							joint.JointData->BoneID = boneId++;

						// pointer to skin mesh animation matrix
						joint.SkinningMatrix = skinMesh->SkinningMatrix + i * 16;
					}
				}

				if (addInvData == false)
				{
					if (GET_ENTITY_DATA(m_root, CWorldInverseTransformData) == NULL)
						m_root->addData<CWorldInverseTransformData>();
					addInvData = true;
				}
			}
		}
	}

	void CRenderMesh::initOptimizeFromPrefab(CEntityPrefab* prefab)
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
				if (!srcRender->isSkinnedMesh())
				{
					// just add static mesh
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

				// init software blendshape
				if (srcRender->getMesh()->BlendShape.size() > 0)
					spawnRender->initSoftwareBlendShape();

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back(GET_ENTITY_DATA(spawnEntity, CWorldTransformData));

				// add world inv transform for culling system (disable to optimize)
				// spawnEntity->addData<CWorldInverseTransformData>();
			}

			// copy culling data
			CCullingData* srcCulling = srcEntity->getData<CCullingData>();
			if (srcCulling != NULL)
			{
				CCullingData* spawnCulling = spawnEntity->addData<CCullingData>();
				spawnCulling->Type = srcCulling->Type;
				spawnCulling->Visible = srcCulling->Visible;
			}
		}

		// for handler on Editor UI
		setEntities(entities, numEntities);
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

	void CRenderMesh::enableInstancing(bool b)
	{
		m_enable = b;
		for (CRenderMeshData*& renderer : m_renderers)
		{
			if (!renderer->isSkinnedMesh())
				renderer->setInstancing(b);
		}
	}

	void CRenderMesh::printEntites()
	{
		std::string log;
		os::Printer::log("[CRenderMesh] printEntites");

		CWorldTransformData* rootTransform = GET_ENTITY_DATA(m_root, CWorldTransformData);

		std::vector<CWorldTransformData*> childs;
		getChildTransforms(rootTransform, childs);

		for (CWorldTransformData* entity : childs)
		{
			log.clear();
			int d = entity->Depth - rootTransform->Depth;
			for (int i = 0; i < d; i++)
				log += "    ";
			log += entity->Name;
			os::Printer::log(log.c_str());
		}
	}

	CWorldTransformData* CRenderMesh::getChildTransform(const char* name)
	{
		for (CWorldTransformData* entity : m_transforms)
		{
			if (entity->Name == name)
			{
				return entity;
			}
		}
		return NULL;
	}

	int CRenderMesh::getChildTransforms(const char* name, std::vector<CWorldTransformData*>& childs)
	{
		CWorldTransformData* findEntity = NULL;

		for (CWorldTransformData* entity : m_transforms)
		{
			if (entity->Name == name)
			{
				findEntity = entity;
				childs.push_back(entity);
				break;
			}
		}

		if (findEntity)
		{
			getChildTransforms(findEntity, childs);
		}

		return (int)childs.size();
	}

	int CRenderMesh::getChildTransforms(CWorldTransformData* transform, std::vector<CWorldTransformData*>& childs, size_t from)
	{
		size_t size = m_transforms.size();

		for (size_t i = from; i < size; i++)
		{
			CWorldTransformData* entity = m_transforms[i];
			if (entity->ParentIndex == transform->EntityIndex)
			{
				childs.push_back(entity);
				getChildTransforms(entity, childs, i + 1);
			}
		}
		return (int)childs.size();
	}
}