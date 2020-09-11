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

	CGameObject* m_currentParticleObj;

	CGUIText *m_label;

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

	void onGUI();

	void onGUIBasic();

	void onGUIZoneNode(int currentZone);

	void onGUIEmitterNode(int currentEmitter);

	void onGUIRendererNode();

	void imguiHelpMarker(const char* desc);

	core::vector3df imguiDirection(core::vector3df baseDirection, float &x, float &y, float &z, const char *name, const char *fmt);

	Particle::CZone* updateParticleZone(Particle::CParticleComponent *particleComponent, Particle::EZone zoneType);

	Particle::CEmitter* updateParticleEmitter(Particle::CParticleComponent *particleComponent, Particle::EEmitter emitterType);

	Particle::IRenderer* updateParticleRenderer(Particle::CParticleComponent *particleComponent);

	Particle::CParticleBufferData* getParticleData(CGameObject *obj);
};