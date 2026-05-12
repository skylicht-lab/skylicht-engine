#pragma once

#include "Material/CMaterial.h"
#include "CLathe.h"

namespace Skylicht
{
	/**
	 * @brief Component for rendering cylinder primitives.
	 * 
	 * ### Example
	 * @code
	 * CGameObject* cylinderObj = zone->createEmptyObject();
	 * CCylinder* cylinder = cylinderObj->addComponent<CCylinder>();
	 * cylinder->setRadius(0.5f);
	 * cylinder->setHeight(2.0f);
	 * cylinder->init(); // Required to rebuild procedural mesh after parameter changes
	 * @endcode
	 * 
	 * @ingroup Primitives
	 */
	class COMPONENT_API CCylinder : public CLathe
	{
	protected:
		float m_radius;
		float m_height;

	public:
		CCylinder();

		virtual ~CCylinder();

		virtual void initComponent();

		virtual void updateComponent();

		void init();

		/**
		 * @brief Set the cylinder radius.
		 * @param r New radius.
		 */
		inline void setRadius(float r)
		{
			m_radius = r;
		}

		/**
		 * @brief Set the cylinder height.
		 * @param h New height.
		 */
		inline void setHeight(float h)
		{
			m_height = h;
		}

		/**
		 * @brief Get the cylinder radius.
		 * @return Radius value.
		 */
		inline float getRadius()
		{
			return m_radius;
		}

		/**
		 * @brief Get the cylinder height.
		 * @return Height value.
		 */
		inline float getHeight()
		{
			return m_height;
		}

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CCylinder)
	};
}