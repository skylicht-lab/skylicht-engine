#pragma once

#include "IApplicationEventReceiver.h"
#include "ParticleSystem/CParticleComponent.h"

class SampleParticlesMagicSkill :
	public IApplicationEventReceiver,
	public IEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;

	CForwardRP *m_forwardRP;

	CGlyphFont* m_font;

	Particle::CParticleComponent* m_projectiles;

public:
	SampleParticlesMagicSkill();

	virtual ~SampleParticlesMagicSkill();

	virtual bool OnEvent(const SEvent& event);

	void initProjectiles(Particle::CParticleComponent *ps);

	void initTower(Particle::CParticleComponent *ps);

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};