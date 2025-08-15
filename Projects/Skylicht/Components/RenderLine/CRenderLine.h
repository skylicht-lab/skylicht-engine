#pragma once

#include "Entity/CEntityHandler.h"
#include "CLineRenderData.h"
#include "Culling/CCullingBBoxData.h"

namespace Skylicht
{
	class COMPONENT_API CRenderLine : public CEntityHandler
	{
	protected:
		CLineRenderData* m_lineData;
		CCullingBBoxData* m_culling;

	public:
		CRenderLine();

		virtual ~CRenderLine();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		virtual CEntity* spawn();

		DECLARE_GETTYPENAME(CRenderLine)

	protected:

		void updateData();

	};
}