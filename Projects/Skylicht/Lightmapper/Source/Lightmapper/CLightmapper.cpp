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

#if defined(USE_OPENMP)
#include <omp.h>
#endif

namespace Skylicht
{
	namespace Lightmapper
	{
		CLightmapper::CLightmapper()
		{
			m_singleBaker = new CBaker();
			m_multiBaker = new CMTBaker();
		}

		CLightmapper::~CLightmapper()
		{
			delete m_singleBaker;
			delete m_multiBaker;
		}

		const CSH9& CLightmapper::bakeAtPosition(
			CCamera *camera, IRenderPipeline* rp, CEntityManager *entityMgr,
			const core::vector3df& position,
			const core::vector3df& normal,
			const core::vector3df& tangent,
			const core::vector3df& binormal,
			int numFace)
		{
			return m_singleBaker->bake(camera, rp, entityMgr, position, normal, tangent, binormal, numFace);
		}

		void CLightmapper::bakeAtPosition(
			CCamera *camera, IRenderPipeline* rp, CEntityManager* entityMgr,
			const core::vector3df *position,
			const core::vector3df *normal,
			const core::vector3df *tangent,
			const core::vector3df *binormal,
			std::vector<CSH9>& out,
			int count,
			int numFace)
		{
			out.clear();

			int maxMT = m_multiBaker->getMaxMT();
			int current = 0;

			while (current < count)
			{
				int numMT = count - current;
				numMT = core::min_(numMT, maxMT);

				// bake and get SH result
				m_multiBaker->bake(camera,
					rp,
					entityMgr,
					position + current,
					normal + current,
					tangent + current,
					binormal + current,
					numMT,
					numFace);

				for (int i = 0; i < numMT; i++)
					out.push_back(m_multiBaker->getSH(i));

				current += numMT;
			}
		}

		void CLightmapper::bakeProbes(std::vector<CProbe*>& probes, CCamera *camera, IRenderPipeline* rp, CEntityManager* entityMgr)
		{
			// prepare comput sh
			core::array<core::vector3df> positions;
			core::array<core::vector3df> normals;
			core::array<core::vector3df> tangents;
			core::array<core::vector3df> binormals;
			for (u32 i = 0, n = probes.size(); i < n; i++)
			{
				CProbe* probe = probes[i];

				core::vector3df pos = probe->getGameObject()->getPosition();
				core::vector3df normal = CTransform::s_oy;
				core::vector3df tangent = CTransform::s_ox;
				core::vector3df binormal = normal.crossProduct(tangent);
				binormal.normalize();

				positions.push_back(pos);
				normals.push_back(normal);
				tangents.push_back(tangent);
				binormals.push_back(binormal);
			}

			// bake sh
			std::vector<CSH9> out;
			CLightmapper::getInstance()->bakeAtPosition(
				camera,
				rp,
				entityMgr,
				positions.pointer(),
				normals.pointer(),
				tangents.pointer(),
				binormals.pointer(),
				out,
				(int)probes.size());

			// apply sh
			for (u32 i = 0, n = probes.size(); i < n; i++)
				probes[i]->setSH(out[i]);
		}

		int CLightmapper::bakeMeshBuffer(IMeshBuffer *mb, const core::matrix4& transform, CCamera *camera, IRenderPipeline* rp, CEntityManager* entityMgr, int begin, int count, core::array<SColor>& outColor, core::array<CSH9>& outSH)
		{
			if (mb->getVertexBufferCount() == 0)
			{
				char log[512];
				sprintf(log, "[CLightmapper] bakeMeshBuffer skip bake MeshBuffer");
				os::Printer::log(log);
				return 0;
			}

			if (mb->getVertexType() != EVT_TANGENTS)
			{
				char log[512];
				sprintf(log, "[CLightmapper] bakeMeshBuffer skip non tangent MeshBuffer");
				os::Printer::log(log);
				return 0;
			}

			IVertexBuffer *vb = mb->getVertexBuffer(0);
			u32 vtxCount = vb->getVertexCount();

			int remain = vtxCount - begin;
			if (remain <= 0)
				return 0;

			count = core::min_(count, remain);

			video::S3DVertexTangents *vtx = (video::S3DVertexTangents*)vb->getVertices();

			core::array<core::vector3df> positions;
			core::array<core::vector3df> normals;
			core::array<core::vector3df> tangents;
			core::array<core::vector3df> binormals;

			positions.set_used(count);
			normals.set_used(count);
			tangents.set_used(count);
			binormals.set_used(count);

			int i = 0;

#pragma omp parallel for private(i)
			for (int id = 0; id < count; id++)
			{
				i = begin + id;

				positions[id] = vtx[i].Pos;
				normals[id] = vtx[i].Normal;
				tangents[id] = vtx[i].Tangent;
				binormals[id] = vtx[i].Binormal;

				transform.transformVect(positions[id]);
				transform.rotateVect(normals[id]);
				transform.rotateVect(tangents[id]);
				transform.rotateVect(binormals[id]);

				normals[id].normalize();
				tangents[id].normalize();
				binormals[id].normalize();
			}

			std::vector<CSH9> resultSH;
			bakeAtPosition(
				camera,
				rp,
				entityMgr,
				positions.pointer(),
				normals.pointer(),
				tangents.pointer(),
				binormals.pointer(),
				resultSH,
				count,
				5);


			core::vector3df result;

#pragma omp parallel for private(result) private(i)
			for (int id = 0; id < count; id++)
			{
				i = begin + id;

				// additive bound light color
				outSH[i] += resultSH[id];

				// get result color
				outSH[i].getSHIrradiance(normals[id], result);

				float r = core::clamp(result.X, 0.0f, 1.0f);
				float g = core::clamp(result.Y, 0.0f, 1.0f);
				float b = core::clamp(result.Z, 0.0f, 1.0f);

				outColor[i].set(
					255, // a
					(int)(r * 255.0f), // r
					(int)(g * 255.0f), // g
					(int)(b * 255.0f)  // b
				);
			}

			return count;
		}
	}
}