#pragma once

#include "RenderPipeline/CForwardRP.h"

namespace Skylicht
{
	namespace Editor
	{
		class CNullForwarderPipeline : public CForwardRP
		{
		protected:

		public:
			CNullForwarderPipeline();

			virtual ~CNullForwarderPipeline();

			virtual void initRender(int w, int h);

			virtual void resize(int w, int h);

			virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP);

			virtual bool canRenderMaterial(CMaterial* material);

			virtual bool canRenderShader(CShader* shader);

			virtual void drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh);

			virtual void drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh);
		};
	}
}