/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "pch.h"
#include "CLightmapper.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CLightmapper::CLightmapper()
		{
			m_rtt = new CBaker();
		}

		CLightmapper::~CLightmapper()
		{
			delete m_rtt;
		}

		const CSH9& CLightmapper::bakeAtPosition(
			CCamera *camera, IRenderPipeline* rp, CEntityManager *entityMgr,
			const core::vector3df& position,
			const core::vector3df& normal,
			const core::vector3df& tangent,
			const core::vector3df& binormal)
		{
			return m_rtt->bake(camera, rp, entityMgr, position, normal, tangent, binormal);
		}
	}
}