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
#include "CQuadRenderer.h"

namespace Skylicht
{
	namespace Particle
	{
		CQuadRenderer::CQuadRenderer() :
			IRenderer(Quad),
			m_atlasNx(1),
			m_atlasNy(1),
			m_billboardType(Camera),
			m_baseShaderType(Additive)
		{
			m_material = new CMaterial("Particle", "BuiltIn/Shader/Particle/ParticleBillboardAdditive.xml");
			m_material->setBackfaceCulling(false);
			m_material->setZWrite(false);

			setMaterialType(m_baseShaderType, m_billboardType);
		}

		CQuadRenderer::~CQuadRenderer()
		{
			delete m_material;
		}

		void CQuadRenderer::setTexture(int slot, ITexture *texture)
		{
			m_material->setTexture(slot, texture);
			m_material->setManualInitTexture(true);
		}

		void CQuadRenderer::setMaterialType(EBaseShaderType shader, EBillboardType billboard)
		{
			if (shader == Additive)
			{
				if (billboard == Camera)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleBillboardAdditive.xml");
				else if (billboard == Velocity)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleVelocityAdditive.xml");
				else if (billboard == FixOrientation)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleOrientationAdditive.xml");
			}
			else if (shader == Transparent)
			{
				if (billboard == Camera)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleBillboardTransparent.xml");
				else if (billboard == Velocity)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleVelocityTransparent.xml");
				else if (billboard == FixOrientation)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleOrientationTransparent.xml");
			}
			else if (shader == AdditiveAlpha)
			{
				if (billboard == Camera)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleBillboardAdditiveAlpha.xml");
				else if (billboard == Velocity)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleVelocityAdditiveAlpha.xml");
				else if (billboard == FixOrientation)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleOrientationAdditiveAlpha.xml");
			}
			else if (shader == TransparentAlpha)
			{
				if (billboard == Camera)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleBillboardTransparentAlpha.xml");
				else if (billboard == Velocity)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleVelocityTransparentAlpha.xml");
				else if (billboard == FixOrientation)
					m_material->changeShader("BuiltIn/Shader/Particle/ParticleOrientationTransparentAlpha.xml");
			}
			else
			{

			}

			m_billboardType = billboard;
			m_baseShaderType = shader;
		}

		void CQuadRenderer::getParticleBuffer(IMeshBuffer *buffer)
		{
			IVertexBuffer *vtx = buffer->getVertexBuffer();
			IIndexBuffer *idx = buffer->getIndexBuffer();

			vtx->set_used(NB_VERTICES_PER_QUAD);
			idx->set_used(NB_INDICES_PER_QUAD);

			// setup indices
			if (idx->getType() == irr::video::EIT_32BIT)
			{
				irr::u32* indices = reinterpret_cast<irr::u32*>(idx->getIndices());
				indices[0] = 0;
				indices[1] = 1;
				indices[2] = 2;

				indices[3] = 0;
				indices[4] = 2;
				indices[5] = 3;
			}
			else if (idx->getType() == irr::video::EIT_16BIT)
			{
				irr::u16* indices = reinterpret_cast<irr::u16*>(idx->getIndices());
				indices[0] = 0;
				indices[1] = 1;
				indices[2] = 2;

				indices[3] = 0;
				indices[4] = 2;
				indices[5] = 3;
			}

			// setup vertices
			irr::video::S3DVertex* vertices = (irr::video::S3DVertex*)vtx->getVertices();
			vertices[0].TCoords = core::vector2df(0.0f, 0.0f);
			vertices[1].TCoords = core::vector2df(1.0f, 0.0f);
			vertices[2].TCoords = core::vector2df(1.0f, 1.0f);
			vertices[3].TCoords = core::vector2df(0.0f, 1.0f);

			video::SColor white(255, 255, 255, 255);

			// top left vertex
			vertices[0].Pos.set(
				-1.0f,
				1.0f,
				0.0f);
			vertices[0].Color = white;

			// top right vertex
			vertices[1].Pos.set(
				1.0f,
				1.0,
				0.0f);
			vertices[1].Color = white;

			// bottom right vertex
			vertices[2].Pos.set(
				1.0f,
				-1.0f,
				0.0f);
			vertices[2].Color = white;

			// bottom left vertex
			vertices[3].Pos.set(
				-1.0f,
				-1.0f,
				0.0f);
			vertices[3].Color = white;

			m_material->addAffectMesh(buffer);
			m_material->applyMaterial();
		}
	}
}