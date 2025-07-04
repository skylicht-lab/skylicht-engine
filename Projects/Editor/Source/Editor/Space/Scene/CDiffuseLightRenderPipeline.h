#pragma once

#include "RenderPipeline/CDeferredRP.h"

namespace Skylicht
{
	namespace Editor
	{
		class CDiffuseLightRenderPipeline : public CDeferredRP
		{
		protected:
			CShader* m_diffuseShader;
			CShader* m_colorShader;
			CShader* m_skinColorShader;

			CMaterial* m_material;

		public:
			CDiffuseLightRenderPipeline();

			virtual ~CDiffuseLightRenderPipeline();

			virtual void initRender(int w, int h);

			virtual bool canRenderMaterial(CMaterial* material);

			virtual bool canRenderShader(CShader* shader);

			virtual void drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh);

			virtual void drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh);
		};
	}
}