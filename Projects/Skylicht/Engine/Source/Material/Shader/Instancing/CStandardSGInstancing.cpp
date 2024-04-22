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
			m_vtxDescriptor->addAttribute("inWorldMatrix1", 4, video::EVAS_TEXCOORD4, video::EVAT_FLOAT, 2);
			m_vtxDescriptor->addAttribute("inWorldMatrix2", 4, video::EVAS_TEXCOORD5, video::EVAT_FLOAT, 2);
			m_vtxDescriptor->addAttribute("inWorldMatrix3", 4, video::EVAS_TEXCOORD6, video::EVAT_FLOAT, 2);
			m_vtxDescriptor->addAttribute("inWorldMatrix4", 4, video::EVAS_TEXCOORD7, video::EVAT_FLOAT, 2);

			m_vtxDescriptor->setInstanceDataStepRate(video::EIDSR_PER_INSTANCE, 1);
			m_vtxDescriptor->setInstanceDataStepRate(video::EIDSR_PER_INSTANCE, 2);
		}

		name = "standard_indirect_instance";
		m_vtxDescriptorForRenderLighting = getVideoDriver()->getVertexDescriptor(name);
		if (m_vtxDescriptorForRenderLighting == NULL)
		{
			// copy new vertex descriptor
			m_vtxDescriptorForRenderLighting = getVideoDriver()->addVertexDescriptor(name);
			for (u32 i = 0; i < m_baseVtxDescriptor->getAttributeCount(); ++i)
			{
				m_vtxDescriptorForRenderLighting->addAttribute(
					m_baseVtxDescriptor->getAttribute(i)->getName(),
					m_baseVtxDescriptor->getAttribute(i)->getElementCount(),
					m_baseVtxDescriptor->getAttribute(i)->getSemantic(),
					m_baseVtxDescriptor->getAttribute(i)->getType(),
					m_baseVtxDescriptor->getAttribute(i)->getBufferID()
				);
			}

			// add color & uv scale
			m_vtxDescriptorForRenderLighting->addAttribute("D0", 3, video::EVAS_TEXCOORD1, video::EVAT_FLOAT, 1);
			m_vtxDescriptorForRenderLighting->addAttribute("D1", 3, video::EVAS_TEXCOORD2, video::EVAT_FLOAT, 1);
			m_vtxDescriptorForRenderLighting->addAttribute("D2", 3, video::EVAS_TEXCOORD3, video::EVAT_FLOAT, 1);
			m_vtxDescriptorForRenderLighting->addAttribute("D3", 3, video::EVAS_TEXCOORD4, video::EVAT_FLOAT, 1);

			// add instance matrix
			m_vtxDescriptorForRenderLighting->addAttribute("inWorldMatrix1", 4, video::EVAS_TEXCOORD5, video::EVAT_FLOAT, 2);
			m_vtxDescriptorForRenderLighting->addAttribute("inWorldMatrix2", 4, video::EVAS_TEXCOORD6, video::EVAT_FLOAT, 2);
			m_vtxDescriptorForRenderLighting->addAttribute("inWorldMatrix3", 4, video::EVAS_TEXCOORD7, video::EVAT_FLOAT, 2);
			m_vtxDescriptorForRenderLighting->addAttribute("inWorldMatrix4", 4, video::EVAS_TEXCOORD8, video::EVAT_FLOAT, 2);

			m_vtxDescriptorForRenderLighting->setInstanceDataStepRate(video::EIDSR_PER_INSTANCE, 1);
			m_vtxDescriptorForRenderLighting->setInstanceDataStepRate(video::EIDSR_PER_INSTANCE, 2);
		}
	}

	CStandardSGInstancing::~CStandardSGInstancing()
	{

	}

	IVertexBuffer* CStandardSGInstancing::createInstancingMeshBuffer()
	{
		return new CVertexBuffer<SVtxSGInstancing>();
	}

	IMeshBuffer* CStandardSGInstancing::createMeshBuffer(video::E_INDEX_TYPE type)
	{
		return new CMeshBuffer<S3DVertex>(m_baseVtxDescriptor, type);
	}

	void CStandardSGInstancing::batchIntancing(
		IVertexBuffer* vtxBuffer,
		CMaterial** materials,
		CEntity** entities,
		int count)
	{
		CVertexBuffer<SVtxSGInstancing>* instanceBuffer = dynamic_cast<CVertexBuffer<SVtxSGInstancing>*>(vtxBuffer);
		if (instanceBuffer == NULL)
			return;

		instanceBuffer->set_used(count);

		CMaterial* material = NULL;
		for (int i = 0; i < count; i++)
		{
			material = materials[i];
			if (material)
			{
				SVtxSGInstancing& vtx = instanceBuffer->getVertex(i);
				CShaderParams& params = material->getShaderParams();

				// convert material data from BuiltIn/Shader/SpecularGlossiness/Deferred/ColorInstancing.xml
				vtx.UVScale = params.getParam(0);
				vtx.Color = params.getParam(1);
				vtx.SpecGloss = params.getParam(2);
			}
		}

		vtxBuffer->setDirty();
	}
}