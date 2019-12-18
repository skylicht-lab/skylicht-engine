#include "IApplicationEventReceiver.h"

#include "Scene/CScene.h"
#include "GameObject/CZone.h"

#include "Camera/CCamera.h"
#include "Camera/CEditorCamera.h"

#include "SkyDome/CSkyDome.h"

#include "RenderPipeline/CForwardRP.h"
#include "RenderPipeline/CDeferredRP.h"

#include "Entity/CEntityManager.h"

class Demo : public IApplicationEventReceiver
{
protected:
	CScene	*m_scene;
	CZone	*m_zone;
	CCamera	*m_camera;

	CBaseRP *m_rendering;

protected:
	io::path getBuiltInPath(const char *name);

public:
	Demo();
	virtual ~Demo();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual void onInitApp();

	virtual void onQuitApp();

protected:

	void initScene();
};