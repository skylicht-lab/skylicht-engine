#pragma once

#include "Entity/CEntityGroup.h"
#include "Entity/IRenderSystem.h"
#include "Transform/CWorldTransformData.h"
#include "CLineRenderData.h"

namespace Skylicht
{
	class COMPONENT_API CLineRenderer : public IRenderSystem
	{
	protected:
		CEntityGroup* m_group;

		CFastArray<CWorldTransformData*> m_transforms;
		CFastArray<CLineRenderData*> m_lines;

	public:
		CLineRenderer();

		virtual ~CLineRenderer();

		virtual void beginQuery(CEntityManager* entityManager);

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		virtual void init(CEntityManager* entityManager);

		virtual void update(CEntityManager* entityManager);

		virtual void render(CEntityManager* entityManager);

		virtual void renderTransparent(CEntityManager* entityManager);

	protected:

		void updateBuffer(CLineRenderData* line, CCamera* camera);

		void renderLine(CEntityManager* entityMgr, CWorldTransformData* transform, CLineRenderData* line);

	};
}