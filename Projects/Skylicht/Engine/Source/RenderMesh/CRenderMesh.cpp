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

		core::array<CEntity*> allEntities;
		CEntity** entities = entityManager->createEntity(numEntities, allEntities);

		// map new entity index from src prefab
		std::map<int, int> entityIndex;

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

					u32 maxJoints = numJoints;
					if (maxJoints < GPU_BONES_COUNT)
						maxJoints = GPU_BONES_COUNT;

					// alloc animation matrix
					skinMesh->SkinningMatrix = new f32[16 * maxJoints];

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

		// for handler on Editor UI
		setEntities(entities, numEntities);
	}

	void CRenderMesh::initOptimizeFromPrefab(CEntityPrefab* prefab)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldTransformData* rootTransform = GET_ENTITY_DATA(m_root, CWorldTransformData);

		// spawn childs entity
		int numEntities = prefab->getNumEntities();

		// we just add the renderer prefab
		core::array<CEntity*> renderEntities;
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

		core::array<CEntity*> allEntities;
		CEntity** entities = entityManager->createEntity(numEntities, allEntities);

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

	void CRenderMesh::removeRenderMeshName(const char* name)
	{
		for (CRenderMeshData*& renderer : m_renderers)
		{
			CWorldTransformData* transform = GET_ENTITY_DATA(renderer->Entity, CWorldTransformData);
			if (transform->Name == name)
			{
				removeRenderMesh(renderer);
				break;
			}
		}
	}

	void CRenderMesh::removeRenderMesh(CRenderMeshData* renderMesh)
	{
		CWorldTransformData* transform = GET_ENTITY_DATA(renderMesh->Entity, CWorldTransformData);

		// drop material
		for (CMaterial* m : m_materials)
			renderMesh->unusedMaterial(m);

		// remove in list
		m_renderTransforms.erase(std::find(m_renderTransforms.begin(), m_renderTransforms.end(), transform));
		m_transforms.erase(std::find(m_transforms.begin(), m_transforms.end(), transform));
		m_renderers.erase(std::find(m_renderers.begin(), m_renderers.end(), renderMesh));

		// remove entity & data
		removeEntity(renderMesh->Entity);
	}

	void CRenderMesh::attachMeshFromPrefab(CEntityPrefab* prefab, std::vector<std::string>& names, ArrayMaterial& materials, bool cloneMaterial, bool resetBoneTransform)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldTransformData* rootTransform = GET_ENTITY_DATA(m_root, CWorldTransformData);

		// index name to transform
		std::map<std::string, CWorldTransformData*> nameToTransform;
		for (CWorldTransformData* transform : m_transforms)
			nameToTransform[transform->Name] = transform;

		// spawn childs entity
		int numEntities = prefab->getNumEntities();

		core::array<CEntity*> renderEntities;
		core::array<CWorldTransformData*> willSpawnEntities;

		// we just add the renderer prefab
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* srcEntity = prefab->getEntity(i);
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();

				// name filter
				if (names.size() == 0)
					renderEntities.push_back(srcEntity);
				else
				{
					for (std::string& name : names)
					{
						if (srcTransform->Name == name)
						{
							renderEntities.push_back(srcEntity);
							break;
						}
					}
				}
			}
		}

		numEntities = renderEntities.size();
		if (numEntities == 0)
			return;

		// insert sort Bone & Mesh to array: willSpawnEntities
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* srcEntity = renderEntities[i];
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();

			bool add = false;
			for (u32 i = 0, n = willSpawnEntities.size(); i < n; i++)
			{
				if (willSpawnEntities[i]->Depth > srcTransform->Depth)
				{
					willSpawnEntities.insert(srcTransform, i);
					add = true;
					break;
				}
			}
			if (!add)
				willSpawnEntities.push_back(srcTransform);

			if (srcRender != NULL)
			{
				if (srcRender->getMesh() && srcRender->isSkinnedMesh())
				{
					// also attach the joints
					CSkinnedMesh* skinMesh = dynamic_cast<CSkinnedMesh*>(srcRender->getMesh());
					u32 numJoints = skinMesh->Joints.size();
					for (u32 i = 0; i < numJoints; i++)
					{
						CSkinnedMesh::SJoint& joint = skinMesh->Joints[i];

						CEntity* srcBoneEntity = prefab->getEntity(joint.EntityIndex);
						CWorldTransformData* srcBone = srcBoneEntity->getData<CWorldTransformData>();

						if (nameToTransform.find(srcBone->Name) == nameToTransform.end())
						{
							add = false;
							for (u32 i = 0, n = willSpawnEntities.size(); i < n; i++)
							{
								if (willSpawnEntities[i]->Depth > srcBone->Depth)
								{
									willSpawnEntities.insert(srcBone, i);
									add = true;
									break;
								}
							}
							if (!add)
								willSpawnEntities.push_back(srcBone);
						}
					}
				}
			}
		}

		numEntities = willSpawnEntities.size();

		core::array<CEntity*> newEntities;
		CEntity** entities = entityManager->createEntity(numEntities, newEntities);

		std::vector<CRenderMeshData*> newRenderers;

		for (int i = 0; i < numEntities; i++)
		{
			CEntity* spawnEntity = entities[i];

			CWorldTransformData* srcTransform = willSpawnEntities[i];
			CEntity* srcEntity = prefab->getEntity(srcTransform->EntityIndex);

			// copy transform data
			{
				CWorldTransformData* spawnTransform = spawnEntity->addData<CWorldTransformData>();
				spawnTransform->Name = srcTransform->Name;

				// update for search later
				nameToTransform[spawnTransform->Name] = spawnTransform;

				// find current parent
				CWorldTransformData* parentTransform = NULL;
				if (srcTransform->ParentIndex != -1)
				{
					CEntity* srcParent = prefab->getEntity(srcTransform->ParentIndex);
					CWorldTransformData* srcParentTransform = srcParent->getData<CWorldTransformData>();

					auto it = nameToTransform.find(srcParentTransform->Name);
					if (it != nameToTransform.end())
						parentTransform = it->second;
				}

				if (parentTransform)
				{
					// add transform as child parent
					spawnTransform->Name = srcTransform->Name;
					spawnTransform->Relative = srcTransform->Relative;
					spawnTransform->HasChanged = true;
					spawnTransform->Depth = rootTransform->Depth + 1 + srcTransform->Depth;
					spawnTransform->ParentIndex = parentTransform->EntityIndex;
				}
				else
				{
					// add transform as child of root
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
				}

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

				newRenderers.push_back(spawnRender);

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

		core::array<CEntity*> allEntity = m_entities;
		for (int i = 0; i < numEntities; i++)
			allEntity.push_back(entities[i]);

		// for handler on Editor UI
		setEntities(allEntity.pointer(), (int)allEntity.size());

		// re-map joint with new entity in CEntityManager
		for (CRenderMeshData*& r : newRenderers)
		{
			if (r->isSkinnedMesh() == true)
			{
				CSkinnedMesh* skinMesh = dynamic_cast<CSkinnedMesh*>(r->getMesh());
				if (skinMesh != NULL)
				{
					u32 numJoints = (u32)skinMesh->Joints.size();

					u32 maxJoints = numJoints;
					if (maxJoints < GPU_BONES_COUNT)
						maxJoints = GPU_BONES_COUNT;

					// alloc animation matrix
					skinMesh->SkinningMatrix = new f32[16 * maxJoints];

					for (u32 i = 0; i < numJoints; i++)
					{
						// map entity data to joint
						CSkinnedMesh::SJoint& joint = skinMesh->Joints[i];

						CEntity* srcBoneEntity = prefab->getEntity(joint.EntityIndex);
						CWorldTransformData* srcBone = srcBoneEntity->getData<CWorldTransformData>();

						CWorldTransformData* transform = nameToTransform[srcBone->Name];

						// or you can call function CRenderMesh::resetDefaultTransform
						if (resetBoneTransform)
							transform->Relative = srcBone->Relative;

						joint.EntityIndex = transform->EntityIndex;
						joint.JointData = GET_ENTITY_DATA(transform->Entity, CJointData);

						// pointer to skin mesh animation matrix
						joint.SkinningMatrix = skinMesh->SkinningMatrix + i * 16;
					}
				}
			}
		}

		// attach materials
		std::vector<CMaterial*> useMaterial;

		for (CMaterial* m : materials)
		{
			CMaterial* mat = m;

			if (cloneMaterial)
				mat = m->clone();
			else
				mat->grab();

			for (CRenderMeshData*& renderer : newRenderers)
			{
				if (renderer->setMaterial(mat) == true)
				{
					m_materials.push_back(mat);
					useMaterial.push_back(mat);
				}
			}
		}

		for (CMaterial* m : useMaterial)
			m->applyMaterial();
	}

	void CRenderMesh::resetDefaultTransform(CEntityPrefab* prefab, std::vector<std::string>& transformsName)
	{
		std::map<std::string, CWorldTransformData*> nameToTransform;
		for (CWorldTransformData* transform : m_transforms)
			nameToTransform[transform->Name] = transform;

		CEntity** entities = prefab->getEntities();
		for (u32 i = 0, n = prefab->getNumEntities(); i < n; i++)
		{
			CEntity* srcEntity = entities[i];
			CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();

			for (std::string& name : transformsName)
			{
				if (srcTransform->Name == name)
				{
					CWorldTransformData* t = nameToTransform[name];
					if (t)
						t->Relative = srcTransform->Relative;
					break;
				}
			}
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