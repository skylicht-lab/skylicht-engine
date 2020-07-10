#pragma once

#include "IApplicationEventReceiver.h"

class SampleShader : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;
	CForwardRP *m_forwardRP;

	CReflectionProbe *m_reflectionProbe;

	bool m_bakeSHLighting;

	ITexture *m_normalMap;

	std::vector<CGameObject*> m_objects;

	CRenderMesh *m_dissolveRenderer;
	float m_dissolveParam;

	CGlyphFont *m_font;

public:
	SampleShader();

	virtual ~SampleShader();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();

protected:

	void initTestNormalMapShader(CEntityPrefab *prefab, ArrayMaterial& materials);

	void initTestReflectionShader(CEntityPrefab *prefab, ArrayMaterial& materials);

	void initTestDissoveShader(CEntityPrefab *prefab, ArrayMaterial& materials);

	void updateDissoveShader();

	void createCanvasText(const char *text, const core::vector3df& position);
};