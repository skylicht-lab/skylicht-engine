#pragma once

#include "CLatheMesh.h"

namespace Skylicht
{
	class COMPONENT_API CCapsuleMesh : public CLatheMesh
	{
	protected:

	public:
		CCapsuleMesh();

		virtual ~CCapsuleMesh();

		void init(float radius, float height, CMaterial* material, bool tangent);

	protected:

		void initOutline(float radius, float height);

	};
}