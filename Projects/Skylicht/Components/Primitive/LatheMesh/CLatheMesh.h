#pragma once

#include "ComponentsConfig.h"
#include "CLine3D.h"
#include "RenderMesh/CMesh.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	/**
	 * @brief Base class for procedurally generating lathe-based meshes (rotational geometry).
	 * @ingroup Primitives
	 */
	class COMPONENT_API CLatheMesh
	{
	protected:
		CLine3D m_outline;
		CMesh* m_mesh;

	public:
		CLatheMesh();

		virtual ~CLatheMesh();

		/**
		 * @brief Get the generated mesh.
		 * @return Pointer to CMesh.
		 */
		inline CMesh* getMesh()
		{
			return m_mesh;
		}

		/**
		 * @brief Draw the rotational outline using debug lines.
		 */
		void drawOutline();

	protected:

		/**
		 * @brief Initialize the lathe mesh by rotating the outline around the Y-axis.
		 * @param material Pointer to the material to apply.
		 * @param tangent True to generate tangent vertices for normal mapping.
		 */
		void initLatheMesh(CMaterial* material, bool tangent);
	};
}