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
#include "CTBNSGInstancing.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	CTBNSGInstancing::CTBNSGInstancing()
	{
		m_baseVtxDescriptor = getVideoDriver()->getVertexDescriptor(EVT_TANGENTS);

		core::stringc name = "tangentbinormal_sg_instance";
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

			// add sh color
			m_vtxDescriptor->addAttribute("sh0", 3, video::EVAS_TEXCOORD4, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("sh1", 3, video::EVAS_TEXCOORD5, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("sh2", 3, video::EVAS_TEXCOORD6, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("sh3", 3, video::EVAS_TEXCOORD7, video::EVAT_FLOAT, 1);

			// add instance matrix
			m_vtxDescriptor->addAttribute("inWorldMatrix1", 4, video::EVAS_TEXCOORD8, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("inWorldMatrix2", 4, video::EVAS_TEXCOORD9, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("inWorldMatrix3", 4, video::EVAS_TEXCOORD10, video::EVAT_FLOAT, 1);
			m_vtxDescriptor->addAttribute("inWorldMatrix4", 4, video::EVAS_TEXCOORD11, video::EVAT_FLOAT, 1);

			m_vtxDescriptor->setInstanceDataStepRate(video::EIDSR_PER_INSTANCE, 1);
		}
	}

	CTBNSGInstancing::~CTBNSGInstancing()
	{

	}

	IVertexBuffer* CTBNSGInstancing::createInstancingMeshBuffer()
	{
		return new CVertexBuffer<SVtxSGInstancing>();
	}

	IMeshBuffer* CTBNSGInstancing::createMeshBuffer(video::E_INDEX_TYPE type)
	{
		return new CMeshBuffer<S3DVertexTangents>(m_baseVtxDescriptor, type);
	}

	void CTBNSGInstancing::batchIntancing(IVertexBuffer* vtxBuffer,
		core::array<CMaterial*>& materials,
		core::array<CWorldTransformData*>& worlds,
		core::array<CIndirectLightingData*> lightings)
	{
		CVertexBuffer<SVtxSGInstancing>* instanceBuffer = dynamic_cast<CVertexBuffer<SVtxSGInstancing>*>(vtxBuffer);
		if (instanceBuffer == NULL)
			return;

		u32 count = worlds.size();
		instanceBuffer->set_used(count);

		CWorldTransformData** worldData = worlds.pointer();
		CMaterial** matData = materials.pointer();
		CIndirectLightingData** lightingData = lightings.pointer();

		float invColor = 1.111f / 255.0f;

		for (u32 i = 0; i < count; i++)
		{
			SVtxSGInstancing& vtx = instanceBuffer->getVertex(i);

			CShaderParams& params = matData[i]->getShaderParams();

			// convert material data from BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormal.xml
			vtx.UVScale = params.getParam(0);
			vtx.Color = params.getParam(1);
			vtx.SpecGloss = params.getParam(2);

			// sh lighting
			CIndirectLightingData* light = lightingData[i];
			if (light->Type == CIndirectLightingData::SH9)
			{
				for (int j = 0; j < 4; j++)
				{
					if (light->SH)
						vtx.SH[j].set(light->SH[j]);
					else
						vtx.SH[j].set(0.0f, 0.0f, 0.0f);
				}
			}
			else
			{
				for (int j = 0; j < 4; j++)
					vtx.SH[j].set(0.0f, 0.0f, 0.0f);

				if (light->Type == CIndirectLightingData::AmbientColor)
				{
					// save ambient to SH0
					vtx.SH[0].set(light->Color.getRed() * invColor, light->Color.getGreen() * invColor, light->Color.getBlue() * invColor);
				}
			}

			// world transform
			vtx.World = worldData[i]->World;
		}

		vtxBuffer->setDirty();
	}
}