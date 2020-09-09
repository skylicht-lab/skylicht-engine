#pragma once

#include "IApplicationEventReceiver.h"

#include "ParticleSystem/CParticleComponent.h"

class SampleParticles : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;

	CForwardRP *m_forwardRP;

	CGlyphFont *m_font;
public:
	SampleParticles();
	virtual ~SampleParticles();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();

protected:

	void createCanvasText(const char *text, const core::vector3df& position);

	void initParticle(Particle::CParticleComponent *particleComponent, Particle::EZone zoneType, Particle::EEmitter emitterType, const core::vector3df& position);

};