#include "pch.h"
#include "CNullForwarderPipeline.h"

namespace Skylicht
{
	namespace Editor
	{
		CNullForwarderPipeline::CNullForwarderPipeline()
		{

		}

		CNullForwarderPipeline::~CNullForwarderPipeline()
		{
		}

		void CNullForwarderPipeline::initRender(int w, int h)
		{

		}

		void CNullForwarderPipeline::resize(int w, int h)
		{

		}

		void CNullForwarderPipeline::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
		{
			if (camera == NULL)
				return;

			IVideoDriver* driver = getVideoDriver();

			ITexture* currentTarget = NULL;
			setTarget(target, cubeFaceId);

			currentTarget = target;

			// custom viewport
			if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
				driver->setViewPort(viewport);

			setCamera(camera);
			entityManager->setCamera(camera);
			entityManager->setRenderPipeline(this);
			entityManager->cullingAndRender();

			onNext(currentTarget, camera, entityManager, viewport, cubeFaceId);
		}

		bool CNullForwarderPipeline::canRenderMaterial(CMaterial* material)
		{
			return true;
		}

		bool CNullForwarderPipeline::canRenderShader(CShader* shader)
		{
			return true;
		}

		void CNullForwarderPipeline::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
		{

		}

		void CNullForwarderPipeline::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
		{

		}
	}
}