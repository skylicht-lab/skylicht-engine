#pragma once

#include "IApplicationEventReceiver.h"
#include "ParticleSystem/CParticleComponent.h"
#include "ParticleSystem/Particles/Systems/CVortexSystem.h"

#include "CTargetProjectile.h"

class SampleParticlesMagicSkill :
	public IApplicationEventReceiver,
	public IEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;

	CForwardRP *m_forwardRP;
	CPostProcessorRP *m_postProcessorRP;

	CGlyphFont* m_font;

	Particle::CVortexSystem *m_vortexSystem;

	CTargetProjectile *m_target;
	Particle::CParticleComponent* m_projectile;
	Particle::CGroup* m_projectileGroup;

	Particle::CParticleComponent* m_impacts;
	Particle::CGroup* m_impactGroup;

	core::vector2df m_mousePosition;
	bool m_mouseDown;
	float m_shootDelay;

public:
	SampleParticlesMagicSkill();

	virtual ~SampleParticlesMagicSkill();

	virtual bool OnEvent(const SEvent& event);

	void initProjectiles(Particle::CParticleComponent *ps);

	void initTower(Particle::CParticleComponent *ps);

	void initImpact(Particle::CParticleComponent *ps);

	void updateProjectile();

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