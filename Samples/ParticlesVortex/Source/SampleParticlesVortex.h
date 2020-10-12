#pragma once

#include "IApplicationEventReceiver.h"
#include "ParticleSystem/CParticleComponent.h"

class SampleParticlesVortex : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;

	CForwardRP *m_forwardRP;
	CGameObject* m_currentParticleObj;	

	Particle::CVortexSystem *m_vortexSystem;

public:
	SampleParticlesVortex();
	virtual ~SampleParticlesVortex();

	void initParticleSystem(Particle::CParticleComponent *ps);

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onResize(int w, int h);

	virtual void onInitApp();

	virtual void onQuitApp();
};