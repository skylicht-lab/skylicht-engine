#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

CViewDemo::CViewDemo()
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext *context = CContext::getInstance();
	CCamera *camera = context->getActiveCamera();
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext *context = CContext::getInstance();
	CScene *scene = context->getScene();
	if (scene != NULL)
		scene->update();
}

bool bake = false;

void CViewDemo::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CCamera *guiCamera = context->getGUICamera();

	CScene *scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->updateDirectionLight();

		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}

	// test bake bake irradiance
	if (bake == false)
	{
		CGameObject *bakeCameraObj = scene->getZone(0)->createEmptyObject();
		CCamera *bakeCamera = bakeCameraObj->addComponent<CCamera>();

		scene->updateAddRemoveObject();

		std::vector<CProbe*>& probes = context->getProbes();
		//for (u32 i = 0, n = probes.size(); i < n; i++)
		//	probes[i]->bakeIrradiance(bakeCamera, context->getRenderPipeline(), scene->getEntityManager());

		// prepare comput sh
		core::array<core::vector3df> positions;
		core::array<core::vector3df> normals;
		core::array<core::vector3df> tangents;
		core::array<core::vector3df> binormals;
		for (u32 i = 0, n = probes.size(); i < n; i++)
		{
			CProbe* probe = probes[i];

			core::vector3df pos = probe->getGameObject()->getPosition();
			core::vector3df normal = CTransform::s_oy;
			core::vector3df tangent = CTransform::s_ox;
			core::vector3df binormal = normal.crossProduct(tangent);
			binormal.normalize();

			positions.push_back(pos);
			normals.push_back(normal);
			tangents.push_back(tangent);
			binormals.push_back(binormal);
		}

		// bake sh
		std::vector<CSH9> out;
		CLightmapper::getInstance()->bakeAtPosition(
			bakeCamera,
			context->getRenderPipeline(),
			scene->getEntityManager(),
			positions.pointer(),
			normals.pointer(),
			tangents.pointer(),
			binormals.pointer(),
			out,
			(int)probes.size());

		// apply sh
		for (u32 i = 0, n = probes.size(); i < n; i++)
			probes[i]->setSH(out[i]);

		bake = true;
	}
}

void CViewDemo::onPostRender()
{

}
