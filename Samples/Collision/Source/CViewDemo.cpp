#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "Projective/CProjective.h"
#include "Debug/CSceneDebug.h"

#include "imgui.h"
#include "CImguiManager.h"

CViewDemo::CViewDemo() :
	m_mouseX(0.0f),
	m_mouseY(0.0f),
	m_decals(NULL),
	m_addDecal(false),
	m_currentTest(0),
	m_decalSizeX(1.0f),
	m_decalSizeY(1.0f),
	m_decalSizeZ(1.0f),
	m_bboxSizeX(5.0f),
	m_bboxSizeY(5.0f),
	m_bboxSizeZ(5.0f),
	m_decalLifeTime(0.0f),
	m_decalRotation(0.0f)
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CEventManager::getInstance()->registerEvent("ViewDemo", this);

	// create decals object
	CZone* zone = context->getActiveZone();
	CGameObject* obj = zone->createEmptyObject();
	m_decals = obj->addComponent<CDecals>();
}

void CViewDemo::onDestroy()
{
	CEventManager::getInstance()->unRegisterEvent(this);
}

bool CViewDemo::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		m_mouseX = (float)event.MouseInput.X;
		m_mouseY = (float)event.MouseInput.Y;

		if (event.MouseInput.isLeftPressed())
		{
			// press
			if (m_currentTest == 2)
			{
				m_addDecal = true;
			}
		}
	}
	return true;
}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	// get view ray from camera to mouse position
	const core::recti& vp = getVideoDriver()->getViewPort();
	core::line3df ray = CProjective::getViewRay(
		context->getActiveCamera(),
		m_mouseX, m_mouseY,
		vp.getWidth(), vp.getHeight()
	);

	float outBestDistanceSquared = 100 * 100; // hit in 100m
	core::vector3df intersection;
	core::triangle3df triangle;
	CCollisionNode* node = NULL;
	CCollisionManager* collisionMgr = context->getCollisionManager();

	if (collisionMgr->getCollisionPoint(ray, outBestDistanceSquared, intersection, triangle, node))
	{
		CSceneDebug* sceneDebug = CSceneDebug::getInstance();

		if (m_currentTest == 0)
		{
			// draw the hit triangle			
			sceneDebug->addTri(triangle, SColor(255, 255, 0, 0));

			// draw intersection point
			core::line3df line;
			line.start = intersection;
			line.end = intersection + triangle.getNormal().normalize();
			sceneDebug->addLine(line, SColor(255, 255, 0, 255));
		}
		else if (m_currentTest == 1)
		{
			// draw bbox query
			core::aabbox3df box;
			core::vector3df halfBox = core::vector3df(m_bboxSizeX * 0.5f, m_bboxSizeY * 0.5f, m_bboxSizeZ * 0.5f);
			box.MinEdge = intersection - halfBox;
			box.MaxEdge = intersection + halfBox;
			sceneDebug->addBoudingBox(box, SColor(255, 0, 255, 0));

			// query
			core::array<core::triangle3df*> listTris;
			core::array<CCollisionNode*> listNodes;

			collisionMgr->getTriangles(box, listTris, listNodes);

			u32 count = listTris.size();
			for (u32 i = 0; i < count; i++)
			{
				sceneDebug->addTri(*listTris[i], SColor(255, 255, 0, 255));
			}
		}
		else if (m_currentTest == 2)
		{
			// draw bbox query
			core::aabbox3df box;
			core::vector3df halfBox = core::vector3df(m_decalSizeX * 0.5f, m_decalSizeY * 0.5f, m_decalSizeZ * 0.5f);
			box.MinEdge = intersection - halfBox;
			box.MaxEdge = intersection + halfBox;
			sceneDebug->addBoudingBox(box, SColor(255, 0, 255, 0));

			// draw intersection point
			core::vector3df normal = triangle.getNormal().normalize();

			core::line3df line;
			line.start = intersection;
			line.end = intersection + normal;
			sceneDebug->addLine(line, SColor(255, 255, 0, 255));

			// add decal when mouse pressed
			if (m_addDecal && m_decals)
			{
				m_decals->addDecal(
					intersection,
					core::vector3df(m_bboxSizeX, m_bboxSizeY, m_bboxSizeZ),
					normal,
					m_decalRotation,
					m_decalLifeTime,
					0.0f);

				m_decals->bake();

				m_addDecal = false;
			}
		}
	}

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	CCamera* guiCamera = context->getGUICamera();

	CScene* scene = context->getScene();

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

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(935, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(320, 200), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Collision Test", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		const char* testCase[] = {
			"Point Collision",
			"BBox Collect",
			"Decal"
		};

		ImGui::Combo("Test case", &m_currentTest, testCase, IM_ARRAYSIZE(testCase));

		if (m_currentTest == 1)
		{
			float minSize = 2.0f;
			float maxSize = 10.0f;

			ImGui::SliderFloat("BBox X", &m_bboxSizeX, minSize, maxSize, "%.3f m");
			ImGui::SliderFloat("BBox Y", &m_bboxSizeY, minSize, maxSize, "%.3f m");
			ImGui::SliderFloat("BBox Z", &m_bboxSizeZ, minSize, maxSize, "%.3f m");
		}
		else if (m_currentTest == 2)
		{
			float minSize = 0.5f;
			float maxSize = 5.0f;

			ImGui::SliderFloat("Decal Size X", &m_decalSizeX, minSize, maxSize, "%.3f m");
			ImGui::SliderFloat("Decal Size Y", &m_decalSizeY, minSize, maxSize, "%.3f m");
			ImGui::SliderFloat("Decal Size Z", &m_decalSizeZ, minSize, maxSize, "%.3f m");

			ImGui::Spacing();

			ImGui::SliderFloat("Lifetime", &m_decalLifeTime, 0.0f, 10.0f, "%.3f sec");
			ImGui::SliderFloat("Rotate", &m_decalRotation, 0.0f, 360.0f, "%.3f deg");

			if (ImGui::Button("Clear all decals"))
			{
				m_decals->removeAllEntities();
			}
		}

		ImGui::End();
	}
}

void CViewDemo::onPostRender()
{

}
