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

		virtual void removeEntity(CEntity* entity);

		virtual void removeAllEntities();

		DECLARE_GETTYPENAME(CRenderLine)

		void loadCustomMaterial(const char* materialFile);

		void setTextureFile(const char* textureFile);

		void setTexture(ITexture* texture);

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

		void setPoints(const std::vector<core::vector3df>& position);

		void setColor(const SColor& c);

		inline const SColor& getColor()
		{
			return m_color;
		}

		inline void setWidth(float w)
		{
			m_lineWidth = w;
			m_lineData->Width = w;
		}

		inline float getWidth()
		{
			return m_lineWidth;
		}

		void setBillboard(bool b);

		inline bool isBillboard()
		{
			return false;
		}

	protected:

		void updateData();

	};
}