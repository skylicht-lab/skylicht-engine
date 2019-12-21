#include "pch.h"
#include "Context/CContext.h"

CContext::CContext() :
	m_scene(NULL),
	m_rendering(NULL),
	m_zone(NULL),
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
	m_rendering = new CForwardRP();
	m_rendering->initRender(w, h);
	return m_rendering;
}

void CContext::releaseRenderPipeline()
{
	if (m_rendering != NULL)
	{
		delete m_rendering;
		m_rendering = NULL;
	}
}