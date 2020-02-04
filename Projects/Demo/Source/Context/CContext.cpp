#include "pch.h"
#include "Context/CContext.h"

CContext::CContext() :
	m_scene(NULL),
	m_rendering(NULL),
	m_zone(NULL),
	m_directionalLight(NULL),
	m_camera(NULL)
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

CBaseRP* CContext::initRenderPipeline(int w, int h)
{
	// 2nd
	m_rendering = new CDeferredRP();
	m_rendering->initRender(w, h);
	m_rendering->enableUpdateEntity(false);

	// 1st
	m_shadowMapRendering = new CShadowMapRP();
	m_shadowMapRendering->initRender(w, h);
	m_shadowMapRendering->setNextPipeLine(m_rendering);

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