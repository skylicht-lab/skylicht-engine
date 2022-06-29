#include "pch.h"
#include "CFBXLoaderFunc.h"

namespace Skylicht
{
	core::matrix4 convertFBXMatrix(ufbx_matrix& mat)
	{
		core::matrix4 ret;

		f32* data = ret.pointer();

		for (int i = 0; i < 4; i++)
		{
			ufbx_vec3& c = mat.cols[i];

			data[i * 4] = (f32)c.x;
			data[i * 4 + 1] = (f32)c.y;
			data[i * 4 + 2] = (f32)c.z;
		}

		return ret;
	}

	core::vector3df convertFBXVec3(ufbx_vec3& v)
	{
		return core::vector3df((f32)v.x, (f32)v.y, (f32)v.z);
	}

	core::vector2df convertFBXVec2(ufbx_vec2& v)
	{
		return core::vector2df((f32)v.x, (f32)v.y);
	}

	core::vector2df convertFBXUVVec2(ufbx_vec2& v)
	{
		return core::vector2df((f32)v.x, 1.0f - (f32)v.y);
	}
}