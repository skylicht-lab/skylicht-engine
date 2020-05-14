#include "pch.h"
#include "CBakeUtils.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		void getWorldView(
			const core::vector3df& normal,
			const core::vector3df& tangent,
			const core::vector3df& binormal,
			const core::vector3df& position,
			int face,
			core::matrix4& out)
		{
			core::vector3df x = tangent;
			core::vector3df y = binormal;
			core::vector3df z = normal;

			if (face == 0)
			{
				// top
				setRow(out, 0, x);
				setRow(out, 1, y);
				setRow(out, 2, z);
			}
			else if (face == 1)
			{
				setRow(out, 0, -z);
				setRow(out, 1, y);
				setRow(out, 2, x);
			}
			else if (face == 2)
			{
				setRow(out, 0, z);
				setRow(out, 1, y);
				setRow(out, 2, -x);
			}
			else if (face == 3)
			{
				setRow(out, 0, x);
				setRow(out, 1, -z);
				setRow(out, 2, y);
			}
			else if (face == 4)
			{
				setRow(out, 0, x);
				setRow(out, 1, z);
				setRow(out, 2, -y);
			}
			else
			{
				// bottom
				setRow(out, 0, x);
				setRow(out, 1, -y);
				setRow(out, 2, -z);
			}

			// translate
			setRow(out, 3, position, 1.0f);
		}

		void setRow(core::matrix4& mat, int row, const core::vector3df& v, float w)
		{
			mat(row, 0) = v.X;
			mat(row, 1) = v.Y;
			mat(row, 2) = v.Z;
			mat(row, 3) = w;
		}
	}
}