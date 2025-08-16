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

		float m_lineWidth;

		SColor m_color;

		bool m_billboard;

		bool m_useCustomMaterial;

		std::string m_textureFile;

		std::string m_materialFile;

		CMaterial* m_material;

		CMaterial* m_customMaterial;

	public:
		CRenderLine();

		virtual ~CRenderLine();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		virtual CEntity* spawn();

		DECLARE_GETTYPENAME(CRenderLine)

		CMaterial* getMaterial();

		inline u32 getNumPoint()
		{
			return (u32)m_entities.size();
		}

		inline CLineRenderData* getLineData()
		{
			return m_lineData;
		}

		CWorldTransformData* getPoint(u32 id);

	protected:

		void updateData();

	};
}