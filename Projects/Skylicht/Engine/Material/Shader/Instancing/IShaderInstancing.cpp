/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "IShaderInstancing.h"

namespace Skylicht
{
	IShaderInstancing::IShaderInstancing() :
		m_baseVtxDescriptor(NULL),
		m_vtxDescriptor(NULL),
		m_vtxDescriptorForRenderLighting(NULL)
	{

	}

	IShaderInstancing::~IShaderInstancing()
	{

	}

	void IShaderInstancing::setupDescriptorForRenderLighting(const char* name)
	{
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

	bool IShaderInstancing::isSupport(IMeshBuffer* mb)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptor)
			return false;

		video::IVertexDescriptor* vtxDes = mb->getVertexDescriptor();

		if (vtxDes != m_baseVtxDescriptor &&
			vtxDes != m_vtxDescriptor)
			return false;

		return true;
	}

	bool IShaderInstancing::isSupport(IMesh* mesh)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptor)
			return false;

		u32 count = mesh->getMeshBufferCount();
		for (u32 i = 0; i < count; i++)
		{
			video::IVertexDescriptor* vtxDesriptor = mesh->getMeshBuffer(i)->getVertexDescriptor();

			if (vtxDesriptor != m_baseVtxDescriptor &&
				vtxDesriptor != m_vtxDescriptor)
				return false;
		}

		return true;
	}

	IVertexBuffer* IShaderInstancing::createTransformVertexBuffer()
	{
		return new CVertexBuffer<SVtxTransform>();
	}

	IVertexBuffer* IShaderInstancing::createIndirectLightingVertexBuffer()
	{
		return new CVertexBuffer<SVtxIndirectLighting>();
	}

	bool IShaderInstancing::applyInstancing(IMeshBuffer* mb, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptor)
			return false;

		mb->setVertexDescriptor(m_vtxDescriptor);

		if (mb->getVertexBufferCount() == 1)
		{
			mb->addVertexBuffer(instancingBuffer);
			mb->addVertexBuffer(transformBuffer);
		}
		else
		{
			mb->setVertexBuffer(instancingBuffer, 1);
			mb->setVertexBuffer(transformBuffer, 2);
		}

		return true;
	}

	bool IShaderInstancing::applyInstancingForRenderLighting(IMeshBuffer* mb, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptorForRenderLighting)
			return false;

		mb->setVertexDescriptor(m_vtxDescriptorForRenderLighting);

		if (mb->getVertexBufferCount() == 1)
		{
			mb->addVertexBuffer(instancingBuffer);
			mb->addVertexBuffer(transformBuffer);
		}
		else
		{
			mb->setVertexBuffer(instancingBuffer, 1);
			mb->setVertexBuffer(transformBuffer, 2);
		}

		return true;
	}

	bool IShaderInstancing::removeInstancing(IMeshBuffer* mb)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptor)
			return false;

		mb->setVertexDescriptor(m_baseVtxDescriptor);
		mb->removeVertexBuffer(1);
		mb->removeVertexBuffer(2);

		return true;
	}

	bool IShaderInstancing::applyInstancing(IMesh* mesh, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptor)
			return false;

		u32 count = mesh->getMeshBufferCount();
		for (u32 i = 0; i < count; i++)
		{
			IMeshBuffer* mb = mesh->getMeshBuffer(i);
			mb->setVertexDescriptor(m_vtxDescriptor);

			if (mb->getVertexBufferCount() == 1)
			{
				mb->addVertexBuffer(instancingBuffer);
				mb->addVertexBuffer(transformBuffer);
			}
			else
			{
				mb->setVertexBuffer(instancingBuffer, 1);
				mb->setVertexBuffer(transformBuffer, 2);
			}
		}

		return true;
	}

	bool IShaderInstancing::applyInstancingForRenderLighting(IMesh* mesh, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptorForRenderLighting)
			return false;

		u32 count = mesh->getMeshBufferCount();
		for (u32 i = 0; i < count; i++)
		{
			IMeshBuffer* mb = mesh->getMeshBuffer(i);
			mb->setVertexDescriptor(m_vtxDescriptorForRenderLighting);

			if (mb->getVertexBufferCount() == 1)
			{
				mb->addVertexBuffer(instancingBuffer);
				mb->addVertexBuffer(transformBuffer);
			}
			else
			{
				mb->setVertexBuffer(instancingBuffer, 1);
				mb->setVertexBuffer(transformBuffer, 2);
			}
		}

		return true;
	}

	bool IShaderInstancing::removeInstancing(IMesh* mesh)
	{
		if (!m_baseVtxDescriptor || !m_vtxDescriptor)
			return false;

		u32 count = mesh->getMeshBufferCount();
		for (u32 i = 0; i < count; i++)
		{
			IMeshBuffer* mb = mesh->getMeshBuffer(i);
			mb->setVertexDescriptor(m_baseVtxDescriptor);
			mb->removeVertexBuffer(1);
			mb->removeVertexBuffer(2);
		}

		return true;
	}

	IMeshBuffer* IShaderInstancing::createLinkMeshBuffer(IMeshBuffer* smb)
	{
		video::E_INDEX_TYPE vtt = smb->getIndexBuffer()->getType();
		IMeshBuffer* dmb = createMeshBuffer(vtt);

		// link vertex & index buffer
		dmb->setVertexBuffer(smb->getVertexBuffer(0));
		dmb->setIndexBuffer(smb->getIndexBuffer());

		return dmb;
	}

	void IShaderInstancing::batchTransformAndLighting(
		IVertexBuffer* tBuffer,
		IVertexBuffer* lBuffer,
		CEntity** entities,
		int count)
	{
		CVertexBuffer<SVtxTransform>* transformBuffer = dynamic_cast<CVertexBuffer<SVtxTransform>*>(tBuffer);
		if (transformBuffer == NULL)
			return;

		CVertexBuffer<SVtxIndirectLighting>* indirectLightBuffer = dynamic_cast<CVertexBuffer<SVtxIndirectLighting>*>(lBuffer);
		if (indirectLightBuffer == NULL)
			return;

		transformBuffer->set_used(count);
		indirectLightBuffer->set_used(count);

		float invColor = 1.111f / 255.0f;

		for (int i = 0; i < count; i++)
		{
			SVtxTransform& transform = transformBuffer->getVertex(i);
			SVtxIndirectLighting& indirectLight = indirectLightBuffer->getVertex(i);

			// world transform
			CWorldTransformData* world = GET_ENTITY_DATA(entities[i], CWorldTransformData);
			transform.World = world->World;

			// indirect lighting
			CIndirectLightingData* indirectLighting = GET_ENTITY_DATA(entities[i], CIndirectLightingData);
			if (indirectLighting)
			{
				switch (indirectLighting->Type)
				{
				case CIndirectLightingData::SH9:
				{
					if (indirectLighting->SH)
					{
						indirectLight.D0 = indirectLighting->SH[0];
						indirectLight.D1 = indirectLighting->SH[1];
						indirectLight.D2 = indirectLighting->SH[2];
						indirectLight.D3 = indirectLighting->SH[3];
					}
				}
				break;
				case CIndirectLightingData::AmbientColor:
				{
					indirectLight.D0.set(
						indirectLighting->Color.getRed() * invColor,
						indirectLighting->Color.getGreen() * invColor,
						indirectLighting->Color.getBlue() * invColor
					);

					indirectLight.D1.set(0.0f, 0.0f, 0.0f);
					indirectLight.D2.set(0.0f, 0.0f, 0.0f);
					indirectLight.D3.set(0.0f, 0.0f, 0.0f);
				}
				break;
				default:
				{
				}
				break;
				}
			}
		}

		tBuffer->setDirty();
		lBuffer->setDirty();
	}

	void IShaderInstancing::batchTransform(
		IVertexBuffer* tBuffer,
		CEntity** entities,
		int count)
	{
		CVertexBuffer<SVtxTransform>* transformBuffer = dynamic_cast<CVertexBuffer<SVtxTransform>*>(tBuffer);
		if (transformBuffer == NULL)
			return;

		transformBuffer->set_used(count);

		for (int i = 0; i < count; i++)
		{
			SVtxTransform& transform = transformBuffer->getVertex(i);

			// world transform
			CWorldTransformData* world = GET_ENTITY_DATA(entities[i], CWorldTransformData);
			transform.World = world->World;
		}

		tBuffer->setDirty();
	}
}