#pragma once

#include "ufbx.h"

namespace Skylicht
{
	core::matrix4 convertFBXMatrix(ufbx_matrix& mat);

	core::vector3df convertFBXVec3(ufbx_vec3& v);

	core::vector2df convertFBXVec2(ufbx_vec2& v);

	core::vector2df convertFBXUVVec2(ufbx_vec2& v);
}