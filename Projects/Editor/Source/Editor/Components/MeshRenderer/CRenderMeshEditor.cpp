/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CRenderMeshEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CRenderMeshEditor, CRenderMesh);

		CRenderMeshEditor::CRenderMeshEditor()
		{

		}

		CRenderMeshEditor::~CRenderMeshEditor()
		{
			clearBlendShape();
		}

		void CRenderMeshEditor::clearBlendShape()
		{
			for (auto b : m_blendshapes)
			{
				delete b;
			}
			m_blendshapes.clear();
		}

		CRenderMeshEditor::SBlendShapeData* CRenderMeshEditor::createGetBlendShapeByName(const char* name)
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

		void CRenderMeshEditor::initGUI(CComponentSystem* target, CSpaceProperty* ui)
		{
			CDefaultEditor::initGUI(target, ui);
		}

		void CRenderMeshEditor::initCustomGUI(GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			CDefaultEditor::initCustomGUI(layout, ui);

			clearBlendShape();

			CRenderMesh* renderMesh = (CRenderMesh*)m_component;
			std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
			for (CRenderMeshData* renderer : renderers)
			{
				core::array<CBlendShape*>& listBlendShape = renderer->getMesh()->BlendShape;
				for (u32 i = 0, n = listBlendShape.size(); i < n; i++)
				{
					CBlendShape* b = listBlendShape[i];
					SBlendShapeData* blendShapeData = createGetBlendShapeByName(b->Name.c_str());
					blendShapeData->BlendShapes.push_back(b);
					blendShapeData->Weight = b->Weight;
				}
			}

			if (m_blendshapes.size() > 0)
			{
				GUI::CCollapsibleGroup* group = ui->addSubGroup(layout);
				group->getHeader()->setLabel(L"Blendshapes");

				GUI::CBoxLayout* childLayout = ui->createBoxLayout(group);

				for (u32 i = 0, n = m_blendshapes.size(); i < n; i++)
				{
					SBlendShapeData* blendShape = m_blendshapes[i];

					CSubject<float>* subject = new CSubject<float>(blendShape->Weight);
					CObserver* observer = new CObserver();
					observer->Notify = [&, blendShape, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								float v = s->get();

								blendShape->Weight = v;
								for (CBlendShape* shape : blendShape->BlendShapes)
								{
									shape->Weight = v;
								}
							}
						};
					subject->addObserver(observer, true);

					std::wstring wname = CStringImp::convertUTF8ToUnicode(blendShape->Name.c_str());
					ui->addSlider(childLayout, wname.c_str(), subject, 0.0f, 1.0f);

					m_subjects.push_back(subject);
				}
			}
		}

		void CRenderMeshEditor::initCustomValueGUI(CObjectSerializable* obj, CValueProperty* data, GUI::CBoxLayout* boxLayout, CSpaceProperty* ui)
		{
			if (data->Name == "lightLayers")
			{
				CUIntProperty* value = dynamic_cast<CUIntProperty*>(data);
				if (value == NULL)
					return;

				initLightLayerMenu(obj, value, boxLayout, ui);
			}
		}

		void CRenderMeshEditor::update()
		{

		}
	}
}