#include "pch.h"
#include "CDiffuseLightRenderPipeline.h"

#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	namespace Editor
	{
		CDiffuseLightRenderPipeline::CDiffuseLightRenderPipeline() :
			m_diffuseShader(NULL),
			m_colorShader(NULL)
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

			m_diffuseShader = shaderMgr->getShaderByName("Diffuse");
			m_colorShader = shaderMgr->getShaderByName("Color");
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
					irrMaterial.MaterialType = m_colorShader->getMaterialRenderID();
				else
					irrMaterial.MaterialType = m_diffuseShader->getMaterialRenderID();

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
				CShader* instancingShader = NULL;

				if (irrMaterial.getTexture(0) == NULL)
				{
					if (size == sizeof(video::S3DVertex))
						instancingShader = m_colorShader->getInstancingShader(video::EVT_STANDARD);
					else if (size == sizeof(video::S3DVertexTangents))
						instancingShader = m_colorShader->getInstancingShader(video::EVT_TANGENTS);
				}
				else
				{
					if (size == sizeof(video::S3DVertex))
						instancingShader = m_diffuseShader->getInstancingShader(video::EVT_STANDARD);
					else
						instancingShader = m_diffuseShader->getInstancingShader(video::EVT_TANGENTS);
				}

				if (instancingShader)
				{
					irrMaterial.MaterialType = instancingShader->getMaterialRenderID();
					driver->setMaterial(irrMaterial);
					driver->drawMeshBuffer(mb);
				}
			}
		}
	}
}