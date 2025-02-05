#pragma once

#include "ComponentsConfig.h"
#include "CLine3D.h"
#include "RenderMesh/CMesh.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	class COMPONENT_API CLatheMesh
	{
	protected:
		CLine3D m_outline;
		CMesh* m_mesh;

	public:
		CLatheMesh();

		virtual ~CLatheMesh();

		inline CMesh* getMesh()
		{
			return m_mesh;
		}

		void drawOutline();

	protected:

		void initLatheMesh(CMaterial* material, bool tangent);
	};
}