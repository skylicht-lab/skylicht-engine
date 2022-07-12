#pragma once

#include "SkylichtEngine.h"

#include "LightProbes/CLightProbe.h"
#include "Lightmapper/CLightmapper.h"

#include "Collision/CCollisionManager.h"

using namespace Lightmapper;

class CContext : public CGameSingleton<CContext>
{
protected:
	CScene* m_scene;

	CZone* m_zone;
	CCamera* m_camera;
	CCamera* m_guiCamera;
	CCollisionManager* m_collisionMgr;

	CDirectionalLight* m_directionalLight;
	std::vector<CPointLight*> m_pointLights;
	std::vector<CLightProbe*> m_probes;

	CBaseRP* m_beginRP;
	CDeferredRP* m_rendering;
	CShadowMapRP* m_shadowMapRendering;
	CForwardRP* m_forwardRP;
	CPostProcessorRP* m_postProcessor;

public:
	CContext();

	virtual ~CContext();

	CScene* initScene();

	CBaseRP* initRenderPipeline(int w, int h, bool postEffect = true);

	CBaseRP* initShadowForwarderPipeline(int w, int h, bool postEffect = true);

	void resize(int w, int h);

	CCollisionManager* getCollisionManager()
	{
		return m_collisionMgr;
	}

	inline CScene* getScene()
	{
		return m_scene;
	}

	inline void setCustomRP(CBaseRP* rp)
	{
		m_beginRP = rp;
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

	inline void setActiveZone(CZone* zone)
	{
		m_zone = zone;
	}

	inline CZone* getActiveZone()
	{
		return m_zone;
	}

	void setDirectionalLight(CDirectionalLight* light);

	void setPointLight(std::vector<CPointLight*> pointLight);

	inline std::vector<CPointLight*>& getPointLight()
	{
		return m_pointLights;
	}

	inline CDirectionalLight* getDirectionalLight()
	{
		return m_directionalLight;
	}

	inline void setActiveCamera(CCamera* camera)
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

	inline void setGUICamera(CCamera* camera)
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
