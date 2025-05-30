#pragma once

#include "Material/CMaterial.h"
#include "CLathe.h"

namespace Skylicht
{
	class COMPONENT_API CCapsule : public CLathe
	{
	protected:
		float m_radius;
		float m_height;

	public:
		CCapsule();

		virtual ~CCapsule();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void init();

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

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CCapsule)
	};
}