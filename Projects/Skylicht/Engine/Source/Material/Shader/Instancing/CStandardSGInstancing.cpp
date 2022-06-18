/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CStandardSGInstancing.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	CStandardSGInstancing::CStandardSGInstancing()
	{
		m_baseVtxDescriptor = getVideoDriver()->getVertexDescriptor(EVT_STANDARD);

		core::stringc name = "standard_sg_instance";
		m_vtxDescriptor = getVideoDriver()->getVertexDescriptor(name);

		if (m_vtxDescriptor == NULL)
		{
			// copy new vertex descriptor
			m_vtxDescriptor = getVideoDriver()->addVertexDescriptor(name);
			for (u32 i = 0; i < m_baseVtxDescriptor->getAttributeCount(); ++i)
			{
				m_vtxDescriptor->addAttribute(
					m_baseVtxDescriptor->getAttribute(i)->getName(),
					m_baseVtxDescriptor->getAttribute(i)->getElementCount(),
					m_baseVtxDescriptor->getAttribute(i)->getSemantic(),
					m_baseVtxDescriptor->getAttribute(i)->getType(),
					m_baseVtxDescriptor->getAttribute(i)->getBufferID()
				);
			}

			// add color & uv scale
			m_vtxDescriptor->addAttribute("uUVScale", 4, video::EVAS_TEXCOORD1, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("uColor", 4, video::EVAS_TEXCOORD2, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("uSpecGloss", 4, video::EVAS_TEXCOORD3, video::EVAT_FLOAT, 1);

			// add instance matrix
			m_vtxDescriptor->addAttribute("inWorldMatrix1", 4, video::EVAS_TEXCOORD4, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("inWorldMatrix2", 4, video::EVAS_TEXCOORD5, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("inWorldMatrix3", 4, video::EVAS_TEXCOORD6, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("inWorldMatrix4", 4, video::EVAS_TEXCOORD7, video::EVAT_FLOAT, 1);

			m_vtxDescriptor->setInstanceDataStepRate(video::EIDSR_PER_INSTANCE, 1);
		}
	}

	CStandardSGInstancing::~CStandardSGInstancing()
	{

	}

	bool CStandardSGInstancing::isSupport(IMeshBuffer* mb)
	{
		if (mb->getVertexDescriptor() == m_baseVtxDescriptor ||
			mb->getVertexDescriptor() == m_vtxDescriptor)
			return true;
		return false;
	}

	IVertexBuffer* CStandardSGInstancing::createInstancingMeshBuffer(IMeshBuffer* sourceMeshBuffer)
	{
		// new instancing vertex buffer
		CVertexBuffer<SVtxSGInstancing>* instanceBuffer = new CVertexBuffer<SVtxSGInstancing>();

		// change new desriptor
		sourceMeshBuffer->setVertexDescriptor(m_vtxDescriptor);

		// add instances buffer
		sourceMeshBuffer->addVertexBuffer(instanceBuffer);

		return instanceBuffer;
	}

	void CStandardSGInstancing::batchIntancing(IVertexBuffer* vtxBuffer,
		core::array<CMaterial*>& materials,
		core::array<core::matrix4>& worlds)
	{
		CVertexBuffer<SVtxSGInstancing>* instanceBuffer = dynamic_cast<CVertexBuffer<SVtxSGInstancing>*>(vtxBuffer);
		if (instanceBuffer == NULL)
			return;

		u32 count = worlds.size();
		instanceBuffer->set_used(count);

		core::matrix4* worldData = worlds.pointer();
		CMaterial** matData = materials.pointer();

		for (u32 i = 0; i < count; i++)
		{
			SVtxSGInstancing& vtx = instanceBuffer->getVertex(i);

			CShaderParams& params = matData[i]->getShaderParams();

			// convert material data from BuiltIn/Shader/SpecularGlossiness/Deferred/ColorInstancing.xml
			vtx.UVScale = params.getParam(0);
			vtx.Color = params.getParam(1);
			vtx.SpecGloss.X = params.getParam(2).X;
			vtx.SpecGloss.Y = params.getParam(3).X;

			// world transform
			vtx.World = worlds[i];
		}
	}
}