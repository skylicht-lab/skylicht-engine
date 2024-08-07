#include "pch.h"
#include "CRenderMeshInstancing.h"

#include "MeshManager/CMeshManager.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "TextureManager/CTextureManager.h"

#include "Transform/CWorldInverseTransformData.h"
#include "Culling/CCullingBBoxData.h"
#include "Culling/CVisibleData.h"

#include "Material/Shader/Instancing/IShaderInstancing.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CRenderMeshInstancing);

	CATEGORY_COMPONENT(CRenderMeshInstancing, "Mesh", "Renderer/Instancing");

	CRenderMeshInstancing::CRenderMeshInstancing() :
		m_root(NULL),
		m_loadTexcoord2(false),
		m_loadNormal(true),
		m_fixInverseNormal(true),
		m_instancingMaterials(NULL),
		m_instancingTransform(NULL),
		m_instancingLighting(NULL),
		m_shareDataTransform(0),
		m_shareDataMaterials(0)
	{

	}

	CRenderMeshInstancing::~CRenderMeshInstancing()
	{
		releaseEntities();
	}

	void CRenderMeshInstancing::releaseBaseEntities()
	{
		if (m_gameObject == NULL)
			return;

		CEntityManager* entityManager = m_gameObject->getEntityManager();
		if (entityManager == NULL)
			return;

		for (int i = (int)m_baseEntities.size() - 1; i >= 0; i--)
			entityManager->removeEntity(m_baseEntities[i]);
		m_baseEntities.clear();
	}

	void CRenderMeshInstancing::releaseMaterial()
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

	void CRenderMeshInstancing::releaseEntities()
	{
		for (CRenderMeshData*& renderer : m_renderers)
		{
			SMeshInstancing* data = renderer->getMeshInstancing();
			if (data != NULL)
			{
				data->ShareDataTransform = NULL;
				data->ShareDataMaterials = NULL;
				data->UseShareMaterialsBuffer = false;
				data->UseShareTransformBuffer = false;
			}
		}

		releaseMaterial();
		removeAllEntities();
		releaseBaseEntities();

		m_baseEntities.clear();
		m_renderers.clear();
		m_entities.clear();

		if (m_instancingMaterials)
		{
			m_instancingMaterials->drop();
			m_instancingMaterials = NULL;
		}

		if (m_instancingTransform)
		{
			m_instancingTransform->drop();
			m_instancingTransform = NULL;
		}

		if (m_instancingLighting)
		{
			m_instancingLighting->drop();
			m_instancingLighting = NULL;
		}
	}

	void CRenderMeshInstancing::initComponent()
	{

	}

	void CRenderMeshInstancing::updateComponent()
	{

	}

	void CRenderMeshInstancing::initFromPrefab(CEntityPrefab* prefab)
	{
		// clear old entity
		releaseEntities();

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

		CEntity** entities = entityManager->createEntity(numEntities, m_baseEntities);

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
				spawnTransform->ParentIndex = m_root->getIndex();

				m_transforms.push_back(spawnTransform);
			}

			// copy render data
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				CWorldTransformData* transform = GET_ENTITY_DATA(spawnEntity, CWorldTransformData);

				CRenderMeshData* spawnRender = spawnEntity->addData<CRenderMeshData>();
				spawnRender->setMesh(srcRender->getMesh());
				spawnRender->setVisible(false);

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back(transform);

				// also check add indirect lighting to hold ReflectionTexture for intacing
				CIndirectLightingData* indirectLighting = spawnEntity->addData<CIndirectLightingData>();
				indirectLighting->initSH();
			}
		}
	}

	CObjectSerializable* CRenderMeshInstancing::createSerializable()
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

	void CRenderMeshInstancing::loadSerializable(CObjectSerializable* object)
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

	void CRenderMeshInstancing::refreshModelAndMaterial()
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

	void CRenderMeshInstancing::initFromMeshFile(const char* path)
	{
		m_meshFile = path;
		refreshModelAndMaterial();
	}

	void CRenderMeshInstancing::initMaterialFromFile(const char* material)
	{
		m_materialFile = material;
		refreshModelAndMaterial();
	}

	void CRenderMeshInstancing::initMaterial(ArrayMaterial& materials, bool cloneMaterial)
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

	void CRenderMeshInstancing::applyShareTransformBuffer()
	{
		if (!m_instancingTransform)
		{
			m_instancingTransform = IShaderInstancing::createTransformVertexBuffer();
			m_instancingTransform->setHardwareMappingHint(EHM_STREAM);
		}

		if (!m_instancingLighting)
		{
			m_instancingLighting = IShaderInstancing::createIndirectLightingVertexBuffer();
			m_instancingLighting->setHardwareMappingHint(EHM_STREAM);
		}

		CMeshManager* meshMgr = CMeshManager::getInstance();
		for (CRenderMeshData*& renderer : m_renderers)
		{
			SMeshInstancing* data = renderer->getMeshInstancing();
			if (data != NULL)
			{
				meshMgr->changeInstancingTransformBuffer(data, m_instancingTransform, m_instancingLighting);
				data->ShareDataTransform = &m_shareDataTransform;
			}
			else
			{
				char logError[1024];
				sprintf(logError, "[CRenderMeshInstancing::applyShareTransformBuffer] renderer not yet init Instancing");
				os::Printer::log(logError);
			}
		}
	}

	void CRenderMeshInstancing::applyShareMaterialBuffer()
	{
		IShaderInstancing* baseInstancing = NULL;

		for (CRenderMeshData*& renderer : m_renderers)
		{
			SMeshInstancing* data = renderer->getMeshInstancing();
			if (data != NULL)
			{
				int numMaterials = data->InstancingShader.size();
				for (int i = 0; i < numMaterials; i++)
				{
					if (baseInstancing == NULL)
						baseInstancing = data->InstancingShader[i];
					else
					{
						if (baseInstancing->getBaseVertexDescriptor() != data->InstancingShader[i]->getBaseVertexDescriptor())
						{
							// can not create shared material
							return;
						}
					}
				}
			}
		}

		if (baseInstancing == NULL)
		{
			char logError[1024];
			sprintf(logError, "[CRenderMeshInstancing::applyShareMaterialBuffer] renderer not yet init Instancing");
			os::Printer::log(logError);
			return;
		}

		if (!m_instancingMaterials)
		{
			m_instancingMaterials = baseInstancing->createInstancingVertexBuffer();
			m_instancingMaterials->setHardwareMappingHint(EHM_STREAM);
		}

		CMeshManager* meshMgr = CMeshManager::getInstance();
		for (CRenderMeshData*& renderer : m_renderers)
		{
			SMeshInstancing* data = renderer->getMeshInstancing();
			if (data != NULL)
			{
				meshMgr->changeInstancingMaterialBuffer(data, m_instancingMaterials);
				data->ShareDataMaterials = &m_shareDataMaterials;
			}
		}
	}

	CEntity* CRenderMeshInstancing::spawn()
	{
		CEntity* entity = createEntity();

		for (CRenderMeshData*& renderer : m_renderers)
		{
			addRendererInstancing(createEntity(entity), m_renderers[0], m_renderTransforms[0]);
		}

		return entity;
	}

	void CRenderMeshInstancing::addRendererInstancing(CEntity* entity, CRenderMeshData* baseRenderMesh, CWorldTransformData* baseTransform)
	{
		CIndirectLightingData* indirect = entity->addData<CIndirectLightingData>();
		indirect->initSH();

		// entity->addData<CWorldInverseTransformData>();
		entity->addData<CCullingData>();
		entity->addData<CVisibleData>();

		// apply instancing at base
		if (!baseRenderMesh->isInstancing())
			baseRenderMesh->setInstancing(true);

		CMesh* mesh = baseRenderMesh->getMesh();

		// set bbox
		CCullingBBoxData* cullingBBox = entity->addData<CCullingBBoxData>();
		cullingBBox->BBox = mesh->getBoundingBox();

		// add render mesh
		CRenderMeshData* renderMesh = entity->addData<CRenderMeshData>();
		renderMesh->setShareMesh(mesh);
		renderMesh->setInstancing(true);

		// apply transform
		CWorldTransformData* transform = entity->getData<CWorldTransformData>();
		transform->Relative = baseTransform->Relative;
		transform->HasChanged = true;
	}
}