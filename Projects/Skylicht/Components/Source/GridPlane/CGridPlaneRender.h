#pragma once

#include "CGridPlaneData.h"
#include "Transform/CWorldTransformData.h"
#include "Entity/IRenderSystem.h"
#include "Entity/CEntityGroup.h"

namespace Skylicht
{
	class CGridPlaneRender : public IRenderSystem
	{
	protected:
		CEntityGroup* m_group;

	public:
		CGridPlaneRender();

		virtual void beginQuery(CEntityManager* entityManager);

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		virtual void init(CEntityManager* entityManager);

		virtual void update(CEntityManager* entityManager);

		virtual void render(CEntityManager* entityManager);
	};
}