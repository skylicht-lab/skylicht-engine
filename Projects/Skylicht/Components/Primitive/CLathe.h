#pragma once

#include "LatheMesh/CLatheMesh.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	/**
	 * @brief Base class for rotational (lathe) primitive components like Capsules and Cylinders.
	 * @ingroup Primitives
	 */
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

		/**
		 * @brief Re-initialize the lathe mesh based on current parameters.
		 */
		virtual void init() = 0;

		/**
		 * @brief Get the generated mesh.
		 * @return Pointer to CMesh.
		 */
		CMesh* getMesh();

		/**
		 * @brief Get the default material.
		 * @return Pointer to CMaterial.
		 */
		inline CMaterial* getMaterial()
		{
			return m_material;
		}

		/**
		 * @brief Get the default material color.
		 * @return SColor value.
		 */
		inline const SColor& getColor()
		{
			return m_color;
		}

		/**
		 * @brief Set the default material color.
		 * @param color New color.
		 */
		void setColor(const SColor& color);

		/**
		 * @brief Enable or disable shadow casting for this object.
		 * @param b True to cast shadows.
		 */
		void setShadowCasting(bool b);

		/**
		 * @brief Check if shadow casting is enabled.
		 * @return True if enabled.
		 */
		inline bool isShadowCasting()
		{
			return m_shadowCasting;
		}

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CLathe)

	protected:

		/**
		 * @brief Initialize the render mesh and culling data for the component's entity.
		 * @param mesh Pointer to the CLatheMesh generator.
		 */
		void initMesh(CLatheMesh* mesh);
	};
}