#pragma once

#include "Lightmapper/Components/Probe/CProbe.h"

using namespace Lightmapper;

class CContext : public CGameSingleton<CContext>
{
protected:
	CScene *m_scene;

	CZone *m_zone;
	CCamera *m_camera;
	CCamera *m_guiCamera;

	CDirectionalLight *m_directionalLight;
	CProbe *m_probe;

	CBaseRP *m_beginRP;
	CBaseRP	*m_rendering;
	CShadowMapRP *m_shadowMapRendering;
	CForwardRP *m_forwardRP;

public:
	CContext();

	virtual ~CContext();

	CScene* initScene();

	CBaseRP* initRenderPipeline(int w, int h);

	inline CScene* getScene()
	{
		return m_scene;
	}

	inline CShadowMapRP* getShadowMapRenderPipeline()
	{
		return m_shadowMapRendering;
	}

	inline CBaseRP* getRenderPipeline()
	{
		return m_beginRP;
	}

	inline CForwardRP* getForwarderRP()
	{
		return m_forwardRP;
	}

	inline void setActiveZone(CZone *zone)
	{
		m_zone = zone;
	}

	inline CZone* getActiveZone()
	{
		return m_zone;
	}

	void updateDirectionLight();

	void setDirectionalLight(CDirectionalLight *light);

	inline CDirectionalLight* getDirectionalLight()
	{
		return m_directionalLight;
	}

	inline void setActiveCamera(CCamera *camera)
	{
		m_camera = camera;
	}

	inline CProbe* getProbe()
	{
		return m_probe;
	}

	inline void setProbe(CProbe *probe)
	{
		m_probe = probe;
	}

	inline CCamera* getActiveCamera()
	{
		return m_camera;
	}

	inline void setGUICamera(CCamera *camera)
	{
		m_guiCamera = camera;
	}

	inline CCamera* getGUICamera()
	{
		return m_guiCamera;
	}

	void releaseScene();

	void releaseRenderPipeline();
};
