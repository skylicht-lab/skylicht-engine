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
#include "Material/Shader/CShaderManager.h"
#include "CParticleInstancing.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleInstancing::CParticleInstancing()
		{
			m_meshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(video::EVT_STANDARD), video::EIT_16BIT);

			video::IVertexDescriptor* baseVertexDescriptor = m_meshBuffer->getVertexDescriptor();
			core::stringc name = baseVertexDescriptor->getName() + "_particle_instance";

			video::IVertexDescriptor* vertexDescriptor = getVideoDriver()->getVertexDescriptor(name);
			if (vertexDescriptor == NULL)
			{
				// create new instancing vertex descriptor
				vertexDescriptor = getVideoDriver()->addVertexDescriptor(name);
				for (u32 i = 0; i < baseVertexDescriptor->getAttributeCount(); ++i)
				{
					vertexDescriptor->addAttribute(
						baseVertexDescriptor->getAttribute(i)->getName(),
						baseVertexDescriptor->getAttribute(i)->getElementCount(),
						baseVertexDescriptor->getAttribute(i)->getSemantic(),
						baseVertexDescriptor->getAttribute(i)->getType(),
						baseVertexDescriptor->getAttribute(i)->getBufferID()
					);
				}

				vertexDescriptor->addAttribute("inParticlePosition", 3, video::EVAS_TEXCOORD1, video::EVAT_FLOAT, 1);
				vertexDescriptor->addAttribute("inParticleColor", 4, video::EVAS_TEXCOORD2, EVAT_UBYTE, 1);
				vertexDescriptor->addAttribute("inParticleUVScale", 2, video::EVAS_TEXCOORD3, video::EVAT_FLOAT, 1);
				vertexDescriptor->addAttribute("inParticleUVOffset", 2, video::EVAS_TEXCOORD4, video::EVAT_FLOAT, 1);
				vertexDescriptor->addAttribute("inParticleSizeRotation", 2, video::EVAS_TEXCOORD5, video::EVAT_FLOAT, 1);

				vertexDescriptor->setInstanceDataStepRate(video::EIDSR_PER_INSTANCE, 1);
			}

			m_meshBuffer->setVertexDescriptor(vertexDescriptor);
			
			m_instanceBuffer = new CVertexBuffer<SParticleInstance>();
			m_meshBuffer->addVertexBuffer(m_instanceBuffer);

			video::SMaterial& material = m_meshBuffer->getMaterial();
			material.MaterialType = CShaderManager::getInstance()->getShaderIDByName("ParticleAdditive");

			m_meshBuffer->setHardwareMappingHint(EHM_STREAM);
			m_instanceBuffer->setHardwareMappingHint(EHM_STREAM);
		}

		CParticleInstancing::~CParticleInstancing()
		{
			m_meshBuffer->drop();
			m_instanceBuffer->drop();
		}
	}
}