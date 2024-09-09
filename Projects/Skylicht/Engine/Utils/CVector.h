/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

namespace Skylicht
{
	class SKYLICHT_API CVector
	{
	public:

		static core::vector3df lerp(const core::vector3df& a, const core::vector3df& b, float t);

		static core::vector3df slerp(const core::vector3df& a, const core::vector3df& b, float t);

		static core::vector3df reflect(const core::vector3df& inDirection, const core::vector3df& normal);

		static float angle(const core::vector3df& a, const core::vector3df& b);

		static core::vector3df project(const core::vector3df& vector, const core::vector3df& onNormal);

		static core::vector3df projectOnPlane(const core::vector3df& vector, const core::vector3df& planeNormal);

		static bool isParallel(const core::vector3df& a, const core::vector3df& b);
	};
}