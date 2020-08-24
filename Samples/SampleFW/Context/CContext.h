#pragma once

#include "SkylichtEngine.h"

#include "Lightmapper/Components/Probe/CLightProbe.h"
#include "Lightmapper/CLightmapper.h"

using namespace Lightmapper;

class CContext : public CGameSingleton<CContext>
{
protected:
	CScene *m_scene;

	CZone *m_zone;
	CCamera *m_camera;
	CCamera *m_guiCamera;

	CDirectionalLight *m_directionalLight;
	std::vector<CLightProbe*> m_probes;

	CBaseRP *m_beginRP;
	CDeferredRP	*m_rendering;
	CShadowMapRP *m_shadowMapRendering;
	CForwardRP *m_forwardRP;
	CPostProcessorRP *m_postProcessor;

public:
	CContext();

	virtual ~CContext();

	CScene* initScene();

	CBaseRP* initRenderPipeline(int w, int h, bool postEffect = true);

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

	inline CPostProcessorRP* getPostProcessorPipeline()
	{
		return m_postProcessor;
	}

	inline CDeferredRP* getDefferredRP()
	{
		return m_rendering;
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

	inline std::vector<CLightProbe*>& getProbes()
	{
		return m_probes;
	}

	inline void setProbes(std::vector<CLightProbe*>& probes)
	{
		m_probes = probes;
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
