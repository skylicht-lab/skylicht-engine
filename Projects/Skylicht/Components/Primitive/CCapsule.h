#pragma once

#include "Material/CMaterial.h"
#include "CLathe.h"

namespace Skylicht
{
	/**
	 * @brief Component for rendering capsule primitives.
	 * 
	 * ### Example
	 * @code
	 * CGameObject* capsuleObj = zone->createEmptyObject();
	 * CCapsule* capsule = capsuleObj->addComponent<CCapsule>();
	 * capsule->setRadius(0.5f);
	 * capsule->setHeight(1.2f);
	 * capsule->init(); // Required to rebuild procedural mesh after parameter changes
	 * @endcode
	 * 
	 * @ingroup Primitives
	 */
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

		/**
		 * @brief Set the radius of the capsule's spherical caps.
		 * @param r New radius.
		 */
		inline void setRadius(float r)
		{
			m_radius = r;
		}

		/**
		 * @brief Set the total height of the capsule.
		 * @param h New height.
		 */
		inline void setHeight(float h)
		{
			m_height = h;
		}

		/**
		 * @brief Get the capsule's radius.
		 * @return Radius value.
		 */
		inline float getRadius()
		{
			return m_radius;
		}

		/**
		 * @brief Get the capsule's total height.
		 * @return Height value.
		 */
		inline float getHeight()
		{
			return m_height;
		}

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CCapsule)
	};
}