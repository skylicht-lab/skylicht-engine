#include "pch.h"
#include "CDiffuseLightRenderPipeline.h"

#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	namespace Editor
	{
		CDiffuseLightRenderPipeline::CDiffuseLightRenderPipeline() :
			m_diffuseShader(0),
			m_colorShader(0),
			m_diffuseInstancingStandardShader(0),
			m_colorInstancingStandardShader(0)
		{

		}

		CDiffuseLightRenderPipeline::~CDiffuseLightRenderPipeline()
		{

		}

		void CDiffuseLightRenderPipeline::initRender(int w, int h)
		{
			CDeferredRP::initRender(w, h);

			CShaderManager* shaderMgr = CShaderManager::getInstance();
			shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
			shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Diffuse.xml");

			m_diffuseShader = shaderMgr->getShaderIDByName("Diffuse");
			m_colorShader = shaderMgr->getShaderIDByName("Color");

			m_diffuseInstancingStandardShader = shaderMgr->getShaderIDByName("DiffuseInstancing");
			m_colorInstancingStandardShader = shaderMgr->getShaderIDByName("ColorInstancing");
		}

		bool CDiffuseLightRenderPipeline::canRenderMaterial(CMaterial* material)
		{
			return true;
		}

		bool CDiffuseLightRenderPipeline::canRenderShader(CShader* shader)
		{
			return true;
		}

		void CDiffuseLightRenderPipeline::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
		{
			if (skinnedMesh)
				return;

			if (m_isIndirectPass == true)
				CDeferredRP::drawMeshBuffer(mesh, bufferID, entity, entityID, skinnedMesh);
			else
			{
				// update texture resource
				updateTextureResource(mesh, bufferID, entity, entityID, skinnedMesh);

				IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
				IVideoDriver* driver = getVideoDriver();

				// force change to pipeline shader
				video::SMaterial irrMaterial = mb->getMaterial();
				if (irrMaterial.getTexture(0) == NULL)
					irrMaterial.MaterialType = m_colorShader;
				else
					irrMaterial.MaterialType = m_diffuseShader;

				// set irrlicht material
				driver->setMaterial(irrMaterial);

				// draw mesh buffer
				driver->drawMeshBuffer(mb);
			}
		}

		void CDiffuseLightRenderPipeline::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
		{
			if (m_isIndirectPass == true)
				CDeferredRP::drawInstancingMeshBuffer(mesh, bufferID, instancingShader, entityMgr, entityID, skinnedMesh);
			else
			{
				IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
				IVideoDriver* driver = getVideoDriver();

				// Note: mesh type is change to Instancing Mesh, so compare size of vertex
				u32 size = mb->getVertexBuffer(0)->getVertexSize();

				video::SMaterial& irrMaterial = mb->getMaterial();

				if (irrMaterial.getTexture(0) == NULL)
				{
					if (size == sizeof(video::S3DVertex))
						irrMaterial.MaterialType = m_colorInstancingStandardShader;
					else if (size == sizeof(video::S3DVertexTangents))
						return;
				}
				else
				{
					if (size == sizeof(video::S3DVertex))
						irrMaterial.MaterialType = m_diffuseInstancingStandardShader;
					else
						return;
				}

				driver->setMaterial(irrMaterial);
				driver->drawMeshBuffer(mb);
			}
		}
	}
}