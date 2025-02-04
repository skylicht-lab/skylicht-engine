#pragma once

#include "CLatheMesh.h"

namespace Skylicht
{
	class COMPONENT_API CCylinderMesh : public CLatheMesh
	{
	protected:

	public:
		CCylinderMesh();

		virtual ~CCylinderMesh();

		void init(float radius, float height, CMaterial* material, bool tangent);

	protected:

		void initOutline(float radius, float height);

	};
}