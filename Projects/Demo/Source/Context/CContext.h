#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "Utils/CGameSingleton.h"

#include "Scene/CScene.h"
#include "RenderPipeline/CForwardRP.h"
#include "RenderPipeline/CDeferredRP.h"

class CContext : public CGameSingleton<CContext>
{
protected:
	CScene *m_scene;
	
	CZone *m_zone;
	CCamera *m_camera;

	CBaseRP	*m_rendering;

public:
	CContext();

	virtual ~CContext();

	CScene* initScene();

	CBaseRP* initRenderPipeline(int w, int h);
	
	inline CScene* getScene()
	{
		return m_scene;
	}

	inline CBaseRP* getRenderPipeline()
	{
		return m_rendering;
	}

	inline void setActiveZone(CZone *zone)
	{
		m_zone = zone;
	}

	inline CZone* getActiveZone()
	{
		return m_zone;
	}

	inline void setActiveCamera(CCamera *camera)
	{
		m_camera = camera;
	}

	inline CCamera* getActiveCamera()
	{
		return m_camera;
	}

	void releaseScene();

	void releaseRenderPipeline();
};

#endif