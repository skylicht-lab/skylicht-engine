#pragma once

#include "LatheMesh/CLatheMesh.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	class COMPONENT_API CLathe : public CComponentSystem
	{
	protected:
		CMaterial* m_material;
		CMaterial* m_customMaterial;

		bool m_shadowCasting;
		bool m_useCustomMaterial;
		bool m_useNormalMap;

		std::string m_materialPath;
		SColor m_color;

		bool m_needReinit;
	public:
		CLathe();

		virtual ~CLathe();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void init() = 0;

		CMesh* getMesh();

		inline CMaterial* getMaterial()
		{
			return m_material;
		}

		inline const SColor& getColor()
		{
			return m_color;
		}

		void setColor(const SColor& color);

		void setShadowCasting(bool b);

		inline bool isShadowCasting()
		{
			return m_shadowCasting;
		}

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CLathe)

	protected:

		void initMesh(CLatheMesh* mesh);
	};
}