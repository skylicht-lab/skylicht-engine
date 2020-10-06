#include "pch.h"
#include "Context/CContext.h"

CContext::CContext() :
	m_scene(NULL),
	m_beginRP(NULL),
	m_rendering(NULL),
	m_shadowMapRendering(NULL),
	m_forwardRP(NULL),
	m_postProcessor(NULL),
	m_zone(NULL),
	m_directionalLight(NULL),
	m_camera(NULL),
	m_guiCamera(NULL)
{

}

CContext::~CContext()
{
	releaseScene();
	releaseRenderPipeline();
}

CScene* CContext::initScene()
{
	m_scene = new CScene();
	return m_scene;
}

void CContext::releaseScene()
{
	if (m_scene != NULL)
	{
		delete m_scene;
		m_scene = NULL;
		m_zone = NULL;
		m_camera = NULL;
	}
}

CBaseRP* CContext::initRenderPipeline(int w, int h, bool postEffect)
{
	// 1st
	m_shadowMapRendering = new CShadowMapRP();
	m_shadowMapRendering->initRender(w, h);

	// 2nd
	m_rendering = new CDeferredRP();
	m_rendering->initRender(w, h);
	m_rendering->enableUpdateEntity(false);

	// 3rd
	m_forwardRP = new CForwardRP(false);
	m_forwardRP->initRender(w, h);
	m_forwardRP->enableUpdateEntity(false);

	// link rp
	m_shadowMapRendering->setNextPipeLine(m_rendering);
	m_rendering->setNextPipeLine(m_forwardRP);

	if (postEffect == true)
	{
		// post processor
		m_postProcessor = new CPostProcessorRP();
		m_postProcessor->enableBloomEffect(true);
		m_postProcessor->enableFXAA(true);
		m_postProcessor->initRender(w, h);

		// apply post processor
		m_rendering->setPostProcessor(m_postProcessor);
	}

	m_beginRP = m_shadowMapRendering;
	return m_beginRP;
}

void CContext::releaseRenderPipeline()
{
	if (m_rendering != NULL)
	{
		delete m_rendering;
		m_rendering = NULL;
	}

	if (m_shadowMapRendering != NULL)
	{
		delete m_shadowMapRendering;
		m_shadowMapRendering = NULL;
	}

	if (m_forwardRP != NULL)
	{
		delete m_forwardRP;
		m_forwardRP = NULL;
	}

	if (m_postProcessor != NULL)
	{
		delete m_postProcessor;
		m_postProcessor = NULL;
	}
}

void CContext::setDirectionalLight(CDirectionalLight *light)
{
	m_directionalLight = light;
}

void CContext::updateDirectionLight()
{
	if (m_shadowMapRendering != NULL && m_directionalLight != NULL)
	{
		m_shadowMapRendering->setLightDirection(m_directionalLight->getDirection());
	}
}
