#include "pch.h"
#include "CRenderLine.h"
#include "CLineRenderer.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "Serializable/CEntityTransformSerializable.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CRenderLine);

	CATEGORY_COMPONENT(CRenderLine, "Render Line", "Renderer");

	CRenderLine::CRenderLine() :
		m_lineData(NULL),
		m_culling(NULL),
		m_lineWidth(1.0f),
		m_billboard(false),
		m_useCustomMaterial(false),
		m_material(NULL),
		m_customMaterial(NULL)
	{

	}

	CRenderLine::~CRenderLine()
	{
		if (m_material)
			m_material->drop();
	}

	void CRenderLine::initComponent()
	{
		CEntity* entity = m_gameObject->getEntity();

		m_lineData = entity->addData<CLineRenderData>();
		m_culling = entity->addData<CCullingBBoxData>();
		entity->addData<CCullingData>();

		m_gameObject->getEntityManager()->addRenderSystem<CLineRenderer>();

		m_material = new CMaterial("Primitive", "BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
		m_material->setUniform4("uColor", m_color);
		m_material->updateShaderParams();

		updateData();
	}

	CMaterial* CRenderLine::getMaterial()
	{
		if (m_useCustomMaterial && m_customMaterial)
			return m_customMaterial;

		return m_material;
	}

	void CRenderLine::updateComponent()
	{

	}

	CObjectSerializable* CRenderLine::createSerializable()
	{
		CObjectSerializable* object = CEntityHandler::createSerializable();

		object->autoRelease(new CFloatProperty(object, "line width", m_lineWidth));
		object->autoRelease(new CBoolProperty(object, "billboard", m_billboard));

		CColorProperty* color = new CColorProperty(object, "color", m_color);
		color->setUIHeader("Default Material");
		object->autoRelease(color);

		CFilePathProperty* material = new CFilePathProperty(object, "material", m_materialFile.c_str(), CMaterialManager::getMaterialExts());
		material->setUIHeader("Custom Material");
		object->autoRelease(material);

		object->autoRelease(new CBoolProperty(object, "custom material", m_useCustomMaterial));

		CArraySerializable* points = new CArraySerializable("Points");
		object->addProperty(points);
		object->autoRelease(points);

		int numProbes = (int)m_entities.size();
		for (int i = 0; i < numProbes; i++)
		{
			CWorldTransformData* world = GET_ENTITY_DATA(m_entities[i], CWorldTransformData);

			CEntityTransformSerializable* entityData = new CEntityTransformSerializable(points);
			points->autoRelease(entityData);

			entityData->Id.set(m_entities[i]->getID());
			entityData->Transform.set(world->Relative);
		}

		return object;
	}

	void CRenderLine::loadSerializable(CObjectSerializable* object)
	{
		CEntityHandler::loadSerializable(object);

		m_lineWidth = object->get("line width", 1.0f);
		m_billboard = object->get("billboard", false);
		m_color = object->get<SColor>("color", SColor(255, 180, 180, 180));
		m_useCustomMaterial = object->get("custom material", false);

		std::string materialFile = object->get<std::string>("material", "");

		CArraySerializable* points = (CArraySerializable*)object->getProperty("Points");
		if (points == NULL)
			return;

		int numPoints = points->getElementCount();

		removeAllEntities();

		for (int i = 0; i < numPoints; i++)
		{
			CEntityTransformSerializable* entityData = (CEntityTransformSerializable*)points->getElement(i);
			if (entityData == NULL)
				return;

			CEntity* entity = createEntity();
			CWorldTransformData* world = GET_ENTITY_DATA(entity, CWorldTransformData);

			// set id
			if (!entityData->Id.get().empty())
				entity->setID(entityData->Id.getString());

			// set transform
			world->Relative = entityData->Transform.get();
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
				m_customMaterial = materials[0];
		}

		m_material->setUniform4("uColor", m_color);
		m_material->updateShaderParams();

		if (m_customMaterial)
		{
			m_customMaterial->setUniform4("uColor", m_color);
			m_customMaterial->updateShaderParams();
		}

		updateData();
	}

	CEntity* CRenderLine::spawn()
	{
		CEntity* entity = createEntity();
		updateData();
		return entity;
	}

	void CRenderLine::updateData()
	{
		m_culling->NeedValidate = true;
		m_lineData->Points.set_used(0);

		for (int i = 0, n = (int)m_entities.size(); i < n; i++)
		{
			CWorldTransformData* t = GET_ENTITY_DATA(m_entities[i], CWorldTransformData);
			m_lineData->Points.push_back(t);

			if (i == 0)
				m_culling->BBox.reset(t->getRelativePosition());
			else
				m_culling->BBox.addInternalPoint(t->getRelativePosition());
		}

		m_lineData->Width = m_lineWidth;
		m_lineData->Billboard = m_billboard;
		m_lineData->Material = getMaterial();
	}
}