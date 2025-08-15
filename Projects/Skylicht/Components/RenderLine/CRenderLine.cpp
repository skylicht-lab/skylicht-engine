#include "pch.h"
#include "CRenderLine.h"
#include "CLineRenderer.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "Serializable/CEntityTransformSerializable.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CRenderLine);

	CATEGORY_COMPONENT(CRenderLine, "Render Line", "Renderer");

	CRenderLine::CRenderLine() :
		m_lineData(NULL),
		m_culling(NULL)
	{

	}

	CRenderLine::~CRenderLine()
	{

	}

	void CRenderLine::initComponent()
	{
		CEntity* entity = m_gameObject->getEntity();

		m_lineData = entity->addData<CLineRenderData>();
		m_culling = entity->addData<CCullingBBoxData>();
		entity->addData<CCullingData>();

		m_gameObject->getEntityManager()->addRenderSystem<CLineRenderer>();
	}

	void CRenderLine::updateComponent()
	{

	}

	CObjectSerializable* CRenderLine::createSerializable()
	{
		CObjectSerializable* object = CEntityHandler::createSerializable();

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
	}
}