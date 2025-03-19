/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CMeshParticleRenderer.h"
#include "MeshManager/CMeshManager.h"
#include "RenderMesh/CRenderMeshData.h"

namespace Skylicht
{
	namespace Particle
	{
		CMeshParticleRenderer::CMeshParticleRenderer() :
			IRenderer(MeshInstancing),
			m_baseShaderType(Soild),
			m_dissolve(0.04),
			m_noiseScale(5.0f, 5.0f, 5.0f),
			m_dissolveColor(255, 255, 150, 75)
		{
			m_meshBuffer = new CMeshBuffer<S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);

			m_material = new CMaterial("Particle", "BuiltIn/Shader/Particle/ParticleMesh.xml");

			setMaterialType(CMeshParticleRenderer::Soild);

			setTexturePath("BuiltIn/Textures/NullTexture.png");
		}

		CMeshParticleRenderer::~CMeshParticleRenderer()
		{
			m_meshBuffer->drop();
			m_material->drop();
		}

		void CMeshParticleRenderer::getParticleBuffer(IMeshBuffer* buffer)
		{
			IVertexBuffer* vtx = buffer->getVertexBuffer();
			IIndexBuffer* idx = buffer->getIndexBuffer();

			vtx->clear();
			idx->clear();

			IIndexBuffer* ib = m_meshBuffer->getIndexBuffer();
			IVertexBuffer* vb = m_meshBuffer->getVertexBuffer();

			for (u32 i = 0, n = vb->getVertexCount(); i < n; i++)
				vtx->addVertex(vb->getVertex(i));

			for (u32 i = 0, n = ib->getIndexCount(); i < n; i++)
				idx->addIndex(ib->getIndex(i));

			m_needUpdateMesh = false;
			m_material->applyMaterial();

			buffer->setDirty();
		}

		CObjectSerializable* CMeshParticleRenderer::createSerializable()
		{
			CObjectSerializable* object = IRenderer::createSerializable();

			CEnumProperty<EBaseShaderType>* shaderType = new CEnumProperty<EBaseShaderType>(object, "shaderType", m_baseShaderType);
			shaderType->setUIHeader("Default Material");
			shaderType->addEnumString("Soild", EBaseShaderType::Soild);
			shaderType->addEnumString("Color", EBaseShaderType::SoildColor);
			shaderType->addEnumString("Additive", EBaseShaderType::Additive);
			shaderType->addEnumString("Transparent", EBaseShaderType::Transparent);
			object->autoRelease(shaderType);

			CVector3Property* noiseScale = new CVector3Property(object, "noiseScale", m_noiseScale);
			noiseScale->setUIHeader("Soild dissolver shader");
			object->autoRelease(noiseScale);
			object->autoRelease(new CFloatProperty(object, "dissolver", m_dissolve, 0.0f, 0.2f));
			object->autoRelease(new CColorProperty(object, "dissolverColor", m_dissolveColor));

			CFilePathProperty* mesh = new CFilePathProperty(object, "mesh", m_meshFile.c_str(), CMeshManager::getMeshExts());
			mesh->setUIHeader("Mesh resource");
			object->autoRelease(mesh);

			return object;
		}

		void CMeshParticleRenderer::loadSerializable(CObjectSerializable* object)
		{
			IRenderer::loadSerializable(object);

			m_noiseScale = object->get<core::vector3df>("noiseScale", core::vector3df(10.0f, 10.0f, 10.0f));
			m_dissolve = object->get<float>("dissolver", 0.04f);
			m_dissolveColor = object->get<SColor>("dissolverColor", SColor(255, 255, 150, 75));

			EBaseShaderType shaderType = object->get<EBaseShaderType>("shaderType", EBaseShaderType::Soild);
			setMaterialType(shaderType);

			std::string meshFile = object->get<std::string>("mesh", "");
			if (meshFile != m_meshFile)
			{
				m_meshFile = meshFile;
				if (!m_meshFile.empty())
					loadMesh(m_meshFile.c_str());
			}
		}

		void CMeshParticleRenderer::setMaterialType(EBaseShaderType shader)
		{
			m_baseShaderType = shader;

			if (m_baseShaderType == Soild)
				m_material->changeShader("BuiltIn/Shader/Particle/ParticleMesh.xml");
			else if (m_baseShaderType == SoildColor)
				m_material->changeShader("BuiltIn/Shader/Particle/ParticleMeshColor.xml");
			else if (m_baseShaderType == Additive)
				m_material->changeShader("BuiltIn/Shader/Particle/ParticleMeshAddtive.xml");
			else if (m_baseShaderType == Transparent)
				m_material->changeShader("BuiltIn/Shader/Particle/ParticleMeshTransparent.xml");

			if (m_baseShaderType == Soild || m_baseShaderType == SoildColor)
			{
				float noiseScale[4];
				noiseScale[0] = m_noiseScale.X;
				noiseScale[1] = m_noiseScale.Y;
				noiseScale[2] = m_noiseScale.Z;
				noiseScale[3] = m_dissolve;
				m_material->setUniform4("uNoiseScale", noiseScale);
				m_material->setUniform4("uDissolveColor", m_dissolveColor);
				m_material->updateShaderParams();
			}
		}

		bool CMeshParticleRenderer::loadMesh(const char* meshFile)
		{
			CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel(
				meshFile,
				"",
				false,
				true,
				false,
				false);

			m_meshBuffer->getVertexBuffer()->clear();
			m_meshBuffer->getIndexBuffer()->clear();

			if (prefab)
			{
				initFromPrefab(prefab);
				m_needUpdateMesh = true;
				return true;
			}

			return false;
		}

		void CMeshParticleRenderer::initFromPrefab(CEntityPrefab* prefab)
		{
			int numEntities = prefab->getNumEntities();

			// we just find the renderer entity
			core::array<CEntity*> renderEntities;
			for (int i = 0; i < numEntities; i++)
			{
				CEntity* srcEntity = prefab->getEntity(i);
				CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
				if (srcRender != NULL && srcRender->getMesh() != NULL)
				{
					if (!srcRender->isSkinnedMesh())
					{
						// just add static mesh
						renderEntities.push_back(srcEntity);
					}
				}
			}

			numEntities = renderEntities.size();

			core::array<core::matrix4> m_transforms;
			core::array<CMesh*> m_meshs;

			for (int i = 0; i < numEntities; i++)
			{
				CEntity* srcEntity = renderEntities[i];

				// get transform data
				CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();
				if (srcTransform != NULL)
				{
					// get the world matrix
					core::matrix4 m = srcTransform->Relative;
					int parentID = srcTransform->ParentIndex;
					while (parentID != -1)
					{
						CEntity* parent = prefab->getEntity(parentID);
						CWorldTransformData* parentTransform = parent->getData<CWorldTransformData>();

						m = parentTransform->Relative * m;
						parentID = parentTransform->ParentIndex;
					}

					m_transforms.push_back(m);
				}

				// get render data
				CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
				if (srcRender != NULL)
				{
					m_meshs.push_back(srcRender->getMesh());
				}
			}

			for (int i = 0; i < numEntities; i++)
			{
				addMesh(m_transforms[i], m_meshs[i]);
			}
		}

		void CMeshParticleRenderer::addMesh(const core::matrix4& transform, CMesh* mesh)
		{
			IVideoDriver* driver = getVideoDriver();

			IIndexBuffer* ib = m_meshBuffer->getIndexBuffer();
			u32 index = ib->getIndexCount();

			IVertexBuffer* vb = m_meshBuffer->getVertexBuffer();

			int numMeshBuffer = mesh->getMeshBufferCount();

			S3DVertex v;

			for (int i = 0; i < numMeshBuffer; i++)
			{
				IMeshBuffer* meshBuffer = mesh->getMeshBuffer(i);

				IVertexBuffer* vertexBuffer = meshBuffer->getVertexBuffer();
				IIndexBuffer* indexBuffer = meshBuffer->getIndexBuffer();

				unsigned char* vertex = (unsigned char*)vertexBuffer->getVertices();
				int vertexSize = vertexBuffer->getVertexSize();

				for (u32 j = 0, n = vertexBuffer->getVertexCount(); j < n; j++)
				{
					unsigned char* vPos = vertex + j * vertexSize;
					video::S3DVertex* vtx = (video::S3DVertex*)vPos;

					v = *vtx;
					transform.transformVect(v.Pos);
					transform.rotateVect(v.Normal);
					v.Normal.normalize();

					vb->addVertex(&v);
				}

				u32 numIndex = indexBuffer->getIndexCount();
				for (u32 j = 0, n = numIndex; j < numIndex; j++)
				{
					ib->addIndex(index + indexBuffer->getIndex(j));
				}
			}
		}
	}
}