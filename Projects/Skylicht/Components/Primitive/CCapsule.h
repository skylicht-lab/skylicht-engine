#pragma once

#include "Material/CMaterial.h"

namespace Skylicht
{
	class COMPONENT_API CCapsule : public CComponentSystem
	{
	protected:
		float m_radius;
		float m_height;

		CMaterial* m_material;
		CMaterial* m_customMaterial;

		bool m_useCustomMaterial;
		bool m_useNormalMap;

		std::string m_materialPath;
		SColor m_color;

	public:
		CCapsule();

		virtual ~CCapsule();

		virtual void initComponent();

		virtual void updateComponent();

		void init();

		inline void setRadius(float r)
		{
			m_radius = r;
		}

		inline void setHeight(float h)
		{
			m_height = h;
		}

		inline float getRadius()
		{
			return m_radius;
		}

		inline float getHeight()
		{
			return m_height;
		}

		inline CMaterial* getMaterial()
		{
			return m_material;
		}

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);
	};
}