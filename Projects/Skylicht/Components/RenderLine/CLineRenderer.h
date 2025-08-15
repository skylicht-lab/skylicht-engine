#pragma once

#include "Entity/CEntityGroup.h"
#include "Entity/IRenderSystem.h"

namespace Skylicht
{
	class COMPONENT_API CLineRenderer : public IRenderSystem
	{
	protected:
		CEntityGroup* m_group;

	public:
		CLineRenderer();

		virtual ~CLineRenderer();

		virtual void beginQuery(CEntityManager* entityManager);

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		virtual void init(CEntityManager* entityManager);

		virtual void update(CEntityManager* entityManager);

		virtual void render(CEntityManager* entityManager);
	};
}