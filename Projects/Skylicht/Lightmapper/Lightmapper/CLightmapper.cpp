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
#include "RenderPipeline/CBaseRP.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		int g_numThread = 128;
		int g_hemisphereBakeSize = 128;

		IMPLEMENT_SINGLETON(CLightmapper);

		CLightmapper::CLightmapper() :
			m_singleBaker(NULL),
			m_multiBaker(NULL),
			m_gpuBaker(NULL)
		{

		}

		CLightmapper::~CLightmapper()
		{
			release();
		}

		void CLightmapper::release()
		{
			if (m_singleBaker != NULL)
			{
				delete m_singleBaker;
				m_singleBaker = NULL;
			}

			if (m_multiBaker != NULL)
			{
				delete m_multiBaker;
				m_multiBaker = NULL;
			}

			if (m_gpuBaker != NULL)
			{
				delete m_gpuBaker;
				m_gpuBaker = NULL;
			}
		}

		void CLightmapper::initBaker(u32 hemisphereBakeSize)
		{
			// adjust size
			u32 size = 16;
			while (size < hemisphereBakeSize && size <= 128)
			{
				size = size * 2;
			}

			if (m_singleBaker != NULL)
				delete m_singleBaker;

			if (m_multiBaker != NULL)
				delete m_multiBaker;

			if (m_gpuBaker != NULL)
				delete m_gpuBaker;

			g_hemisphereBakeSize = size;

			m_singleBaker = new CBaker();
			m_multiBaker = new CMTBaker();
			m_gpuBaker = new CGPUBaker();
		}

		const CSH9& CLightmapper::bakeAtPosition(
			CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr,
			const core::vector3df& position,
			const core::vector3df& normal,
			const core::vector3df& tangent,
			const core::vector3df& binormal,
			int numFace)
		{
			if (m_singleBaker == NULL)
			{
				os::Printer::log("[CLightmapper::bakeAtPosition] Need call initBaker first");
				return m_temp;
			}

			return m_singleBaker->bake(camera, rp, entityMgr, position, normal, tangent, binormal, numFace);
		}

		void CLightmapper::bakeAtPosition(
			CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr,
			const core::vector3df* position,
			const core::vector3df* normal,
			const core::vector3df* tangent,
			const core::vector3df* binormal,
			std::vector<CSH9>& out,
			int count,
			int numFace)
		{
			out.clear();

			if (m_multiBaker == NULL)
			{
				os::Printer::log("[CLightmapper::bakeAtPosition] Need call initBaker first");
				return;
			}

			// default use multi thread bakder
			CMTBaker* baker = m_multiBaker;

			// switch gpu if supported
			if (m_gpuBaker->canUseGPUBaker() == true)
				baker = m_gpuBaker;

			int maxMT = baker->getMaxMT();
			int current = 0;

			while (current < count)
			{
				int numMT = count - current;
				numMT = core::min_(numMT, maxMT);

				// bake and get SH result
				baker->bake(camera,
					rp,
					entityMgr,
					position + current,
					normal + current,
					tangent + current,
					binormal + current,
					numMT,
					numFace);

				for (int i = 0; i < numMT; i++)
					out.push_back(baker->getSH(i));

				current += numMT;
			}
		}

		void CLightmapper::bakeProbes(std::vector<CLightProbe*>& probes, CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr)
		{
			// prepare comput sh
			core::array<core::vector3df> positions;
			core::array<core::vector3df> normals;
			core::array<core::vector3df> tangents;
			core::array<core::vector3df> binormals;
			for (u32 i = 0, n = (u32)probes.size(); i < n; i++)
			{
				CLightProbe* probe = probes[i];

				core::vector3df pos = probe->getGameObject()->getPosition();
				core::vector3df normal = Transform::Oy;
				core::vector3df tangent = Transform::Ox;
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
			for (u32 i = 0, n = (u32)probes.size(); i < n; i++)
			{
				out[i].copyTo(probes[i]->getSH9());
				probes[i]->needValidate();
			}
		}

		void CLightmapper::bakeProbes(std::vector<core::vector3df>& position, std::vector<CSH9>& probes, CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr)
		{
			// prepare comput sh
			core::array<core::vector3df> positions;
			core::array<core::vector3df> normals;
			core::array<core::vector3df> tangents;
			core::array<core::vector3df> binormals;

			for (u32 i = 0, n = (u32)position.size(); i < n; i++)
			{
				core::vector3df pos = position[i];
				core::vector3df normal = Transform::Oy;
				core::vector3df tangent = Transform::Ox;
				core::vector3df binormal = normal.crossProduct(tangent);
				binormal.normalize();

				positions.push_back(pos);
				normals.push_back(normal);
				tangents.push_back(tangent);
				binormals.push_back(binormal);
			}

			// bake sh			
			CLightmapper::getInstance()->bakeAtPosition(
				camera,
				rp,
				entityMgr,
				positions.pointer(),
				normals.pointer(),
				tangents.pointer(),
				binormals.pointer(),
				probes,
				(int)position.size());
		}

		int CLightmapper::bakeMeshBuffer(IMeshBuffer* mb, const core::matrix4& transform, CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr, int begin, int count, core::array<SColor>& outColor, core::array<CSH9>& outSH)
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

			IVertexBuffer* vb = mb->getVertexBuffer(0);
			u32 vtxCount = vb->getVertexCount();

			int remain = vtxCount - begin;
			if (remain <= 0)
				return 0;

			count = core::min_(count, remain);

			video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)vb->getVertices();

			core::array<core::vector3df> positions;
			core::array<core::vector3df> normals;
			core::array<core::vector3df> tangents;
			core::array<core::vector3df> binormals;

			positions.set_used(count);
			normals.set_used(count);
			tangents.set_used(count);
			binormals.set_used(count);

			int i;

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

				// move 2cm
				positions[id] += normals[id] * 0.02f;
			}

			CBaseRP::setBakeLightmapMode(true);

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

			CBaseRP::setBakeLightmapMode(false);

			core::vector3df result;
			float r, g, b;

#pragma omp parallel for private(result, i, r, g, b)
			for (int id = 0; id < count; id++)
			{
				i = begin + id;

				outSH[i] = resultSH[id];

				// get result color
				outSH[i].getSHIrradiance(normals[id], result);

				// dark multipler
				float l = 1.0f - core::clamp(0.21f * result.X + 0.72f * result.Y + 0.07f * result.Z, 0.0f, 1.0f);

				// use QuadraticEaseIn function (y = x^2) or CubicEaseIn (y = x^3)
				// [x -> 0.0 - 1.0] 
				// [y -> 1.0 - 1.5]
				float darkMultipler = 1.0f + 1.5f * l * l * l;

				// compress lighting by 3.0
				result *= darkMultipler / 3.0f;

				r = core::clamp(result.X, 0.0f, 1.0f);
				g = core::clamp(result.Y, 0.0f, 1.0f);
				b = core::clamp(result.Z, 0.0f, 1.0f);

				outColor[i].set(
					255, // a
					(int)(r * 255.0f), // r
					(int)(g * 255.0f), // g
					(int)(b * 255.0f)  // b
				);
			}

			return count;
		}

		void CLightmapper::setNumThread(u32 num)
		{
			g_numThread = num;
		}

		void CLightmapper::setHemisphereBakeSize(u32 size)
		{
			g_hemisphereBakeSize = size;
		}

		u32 CLightmapper::getNumThread()
		{
			return g_numThread;
		}

		u32 CLightmapper::getHemisphereBakeSize()
		{
			return g_hemisphereBakeSize;
		}
	}
}