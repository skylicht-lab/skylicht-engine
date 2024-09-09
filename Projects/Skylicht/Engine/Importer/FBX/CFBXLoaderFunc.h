#pragma once

#include "ufbx.h"

namespace Skylicht
{
	core::matrix4 convertFBXMatrix(const ufbx_matrix& mat);

	core::vector3df convertFBXVec3(const ufbx_vec3& v);

	core::vector2df convertFBXVec2(const ufbx_vec2& v);

	core::vector2df convertFBXUVVec2(const ufbx_vec2& v);
}