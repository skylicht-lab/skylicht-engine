/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "CPrimiviteData.h"
#include "Entity/CEntityHandler.h"
#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	/**
	 * @brief Base class for primitive shape components (Cube, Sphere, Plane).
	 * Supports multi-instance rendering within a single component, custom materials, and hardware instancing.
	 * 
	 * @note By default, primitives use **Deferred shaders**, which means they will only render 
	 * on a **Deferred Pipeline**. If you are using a **Forward Pipeline** (common on mobile), 
	 * you must call setCustomMaterial() with a material that uses a forward-compatible shader.
	 * 
	 * ### Example
	 * @code
	 * // Create a component that manages multiple cube instances
	 * CGameObject* cubeManager = zone->createEmptyObject();
	 * CCube* cubes = cubeManager->addComponent<CCube>();
	 * cubes->setInstancing(true); // Enable hardware instancing for performance
	 * 
	 * // Add individual primitives at different positions/rotations
	 * cubes->addPrimitive(core::vector3df(0, 0, 0), core::vector3df(0, 0, 0), core::vector3df(1, 1, 1));
	 * cubes->addPrimitive(core::vector3df(2, 0, 0), core::vector3df(45, 45, 0), core::vector3df(1, 2, 1));
	 * @endcode
	 * 
	 * @ingroup Primitives
	 */
	class COMPONENT_API CPrimitive : public CEntityHandler
	{
	protected:
		CPrimiviteData::EPrimitive m_type;

		SColor m_color;

		bool m_instancing;

		bool m_useCustomMaterial;

		bool m_useNormalMap;

		CMaterial* m_material;

		CMaterial* m_customMaterial;

		std::string m_materialPath;

	protected:

		CPrimitive();

		virtual ~CPrimitive();

		virtual void initComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		virtual CEntity* spawn();

	public:

		/**
		 * @brief Add a primitive instance to this component.
		 * @param pos Relative position.
		 * @param rotDeg Relative rotation in degrees.
		 * @param scale Relative scale.
		 * @return Pointer to the created entity.
		 */
		CEntity* addPrimitive(const core::vector3df& pos, const core::vector3df& rotDeg, const core::vector3df& scale);

		/**
		 * @brief Get the active material.
		 * @return Pointer to CMaterial.
		 * @note By default, this returns the internal material which is loaded with a **Deferred shader**.
		 * It will not render correctly on a Forward Pipeline unless a custom material is set.
		 */
		CMaterial* getMaterial();

		/**
		 * @brief Get the internal mesh used for rendering.
		 * @return Pointer to CMesh.
		 */
		CMesh* getMesh();

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
		 * @brief Set a custom material for all instances.
		 * @param material Pointer to CMaterial.
		 */
		void setCustomMaterial(CMaterial* material);

		/**
		 * @brief Enable or disable hardware instancing.
		 * @param b True to enable instancing.
		 */
		void setInstancing(bool b);

		/**
		 * @brief Check if hardware instancing is enabled.
		 * @return True if enabled.
		 */
		inline bool isInstancing()
		{
			return m_instancing;
		}

		/**
		 * @brief Enable or disable normal map support (uses tangent vertices).
		 * @param b True to enable.
		 */
		inline void setEnableNormalMap(bool b)
		{
			m_useNormalMap = b;
		}

		/**
		 * @brief Check if normal map is enabled.
		 * @return True if enabled.
		 */
		inline bool isUseNormalMap()
		{
			return m_useNormalMap;
		}

		/**
		 * @brief Get the primitive type.
		 * @return EPrimitive value.
		 */
		inline CPrimiviteData::EPrimitive getType()
		{
			return m_type;
		}

		DECLARE_GETTYPENAME(CPrimitive)
	};
}