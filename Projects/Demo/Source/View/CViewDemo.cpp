#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "Graphics2D/CGraphics2D.h"

#include "Material/Shader/CShaderManager.h"

CViewDemo::CViewDemo()
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext *context = CContext::getInstance();
	CCamera *camera = context->getActiveCamera();
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext *context = CContext::getInstance();
	CScene *scene = context->getScene();
	if (scene != NULL)
		scene->update();
}

void CViewDemo::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CScene *scene = context->getScene();

	if (camera != NULL && scene != NULL)
	{
		context->updateDirectionLight();

		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager());

		/*
		SColor color(255, 255, 255, 255);
		ITexture *texture = context->getShadowMapRenderPipeline()->getDepthTexture();
		CGraphics2D *g = CGraphics2D::getInstance();
		g->begin2D();
		int shaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColor");
		g->addImageBatch(texture, color, core::IdentityMatrix, shaderID);
		g->flush();
		*/
	}
}