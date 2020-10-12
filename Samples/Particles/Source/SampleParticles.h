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

	CGUIText *m_label;
	CGlyphFont *m_font;

	CGameObject* m_currentParticleObj;

	Particle::CGroup* m_particleGroup;
	Particle::CZone* m_particleZone;
	Particle::CEmitter* m_particleEmitter;

public:

	SampleParticles();

	virtual ~SampleParticles();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onResize(int w, int h);

	virtual void onInitApp();

	virtual void onQuitApp();

protected:

	void createCanvasText(const char *text, const core::vector3df& position);

	void onGUI();

	void onGUIParticle();

	void onGUIZoneNode(int currentZone);

	void onGUIEmitterNode(int currentEmitter);

	void onGUIRendererNode();

	void imguiHelpMarker(const char* desc);

	core::vector3df imguiDirection(core::vector3df baseDirection, float &x, float &y, float &z, const char *name, const char *fmt);

	Particle::CGroup* updateParticleGroup(Particle::CParticleComponent *particleComponent);

	Particle::CZone* updateParticleZone(Particle::CParticleComponent *particleComponent, Particle::EZone zoneType);

	Particle::CEmitter* updateParticleEmitter(Particle::CParticleComponent *particleComponent, Particle::EEmitter emitterType);

	Particle::IRenderer* updateParticleRenderer(Particle::CParticleComponent *particleComponent);

	Particle::IRenderer* updateParticleRendererType(Particle::CParticleComponent *particleComponent, int typeId);
};