#include "pch.h"
#include "CViewDemo.h"

#include "imgui.h"
#include "CImguiManager.h"

#include "Context/CContext.h"

CViewDemo::CViewDemo() :
	m_cat(NULL)
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CScene* scene = context->getScene();

	scene->updateIndexSearchObject();
	m_cat = scene->searchObjectInChild(L"Cat");

	if (m_cat)
	{
		CRenderMesh* renderMesh = m_cat->getComponent<CRenderMesh>();

		// get all render nodes and get blendshape data in mesh file
		std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
		for (CRenderMeshData* renderer : renderers)
		{
			core::array<CBlendShape*>& blendShapes = renderer->getMesh()->BlendShape;
			for (u32 i = 0, n = blendShapes.size(); i < n; i++)
			{
				CBlendShape* blendShape = blendShapes[i];

				SBlendShapeData* blendShapeData = createGetBlendShapeByName(blendShape->Name.c_str());
				blendShapeData->Name = blendShape->Name;
				blendShapeData->Weight = blendShape->Weight;
				blendShapeData->BlendShapes.push_back(blendShape);
			}
		}
	}
}

void CViewDemo::onDestroy()
{
	for (SBlendShapeData* blendShape : m_blendshapes)
		delete blendShape;

	m_blendshapes.clear();
}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

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
	ImGui::SetNextWindowSize(ImVec2(310, 270), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("BlendShape Test", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		if (ImGui::CollapsingHeader("BlendShape", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float minSize = 0.0f;
			float maxSize = 1.0f;

			for (SBlendShapeData* blendShape : m_blendshapes)
			{
				// show all blendshape data control
				ImGui::SliderFloat(blendShape->Name.c_str(), &blendShape->Weight, minSize, maxSize, "%.3f");

				// update the blendshape weight
				for (CBlendShape* shape : blendShape->BlendShapes)
				{
					shape->Weight = blendShape->Weight;
				}
			}
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Animations", ImGuiTreeNodeFlags_DefaultOpen))
		{

		}

		ImGui::End();
	}
}

void CViewDemo::onPostRender()
{

}

CViewDemo::SBlendShapeData* CViewDemo::createGetBlendShapeByName(const char* name)
{
	for (SBlendShapeData* blendShape : m_blendshapes)
	{
		if (blendShape->Name == name)
		{
			return blendShape;
		}
	}

	SBlendShapeData* newBlendShape = new SBlendShapeData();
	newBlendShape->Name = name;
	m_blendshapes.push_back(newBlendShape);
	return newBlendShape;
}