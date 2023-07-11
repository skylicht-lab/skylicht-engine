#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"

#include "Lightmapper/CLightmapper.h"
#include "Lightmapper/CSH9.h"

#include "BakeDirectionalRP/CShadowMapBakeRP.h"
#include "BakeDirectionalRP/CDirectionalLightBakeRP.h"

#include "BakePointLightRP/CPointLightShadowBakeRP.h"
#include "BakePointLightRP/CPointLightBakeRP.h"

#include "Rasterisation/CRasterisation.h"

#define MAX_LIGHTMAP_ATLAS 10

// #define LIGHTMAP_SPONZA

class CViewBakeLightmap : public CView
{
protected:
	std::vector<CRenderMesh*> m_renderMesh;
	std::vector<CLight*> m_lights;

	std::vector<IMeshBuffer*> m_meshBuffers;
	std::vector<core::matrix4> m_meshTransforms;

	ITexture* m_directionLightBake[MAX_LIGHTMAP_ATLAS];
	ITexture* m_pointLightBake[MAX_LIGHTMAP_ATLAS];
	ITexture* m_result[MAX_LIGHTMAP_ATLAS];

	IMeshBuffer* m_subMesh[MAX_LIGHTMAP_ATLAS];

	int m_numBakeTexture;
	int m_currentMesh;

	CGameObject* m_bakeCameraObject;
	CGameObject* m_guiObject;
	CGlyphFont* m_font;
	CGUIText* m_textInfo;

	int m_lightmapSize;
	int m_numRenderers;

	CShadowMapBakeRP* m_shadowRP;
	CDirectionalLightBakeRP* m_bakeLightRP;

	CPointLightShadowBakeRP* m_shadowPLRP;
	CPointLightBakeRP* m_bakePointLightRP;
public:

	CViewBakeLightmap();

	virtual ~CViewBakeLightmap();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

public:

	void gotoDemoView();
};