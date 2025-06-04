/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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
#include "CBakeLightComponent.h"

#include "Material/CMaterialManager.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		ACTIVATOR_REGISTER(CBakeLightComponent);

		CATEGORY_COMPONENT(CBakeLightComponent, "Bake Light", "Lightmapper");

		CBakeLightComponent::CBakeLightComponent() :
			m_bakeSize(Size2048),
			m_outputFile("lightmap_directional_%d.png")
		{

		}

		CBakeLightComponent::~CBakeLightComponent()
		{

		}

		void CBakeLightComponent::initComponent()
		{

		}

		void CBakeLightComponent::updateComponent()
		{

		}

		CObjectSerializable* CBakeLightComponent::createSerializable()
		{
			CObjectSerializable* object = CComponentSystem::createSerializable();

			object->autoRelease(new CFolderPathProperty(object, "folder", m_outputFolder.c_str()));
			object->autoRelease(new CStringProperty(object, "file", m_outputFile.c_str()));

			CEnumProperty<EBakeSize>* bakeSize = new CEnumProperty<EBakeSize>(object, "size", m_bakeSize);
			bakeSize->addEnumString("512", EBakeSize::Size512);
			bakeSize->addEnumString("1024", EBakeSize::Size1024);
			bakeSize->addEnumString("2048", EBakeSize::Size2048);
			bakeSize->addEnumString("4096", EBakeSize::Size4096);
			object->autoRelease(bakeSize);

			return object;
		}

		void CBakeLightComponent::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);

			m_outputFolder = object->get<std::string>("folder", "");
			m_outputFile = object->get<std::string>("file", "");
			m_bakeSize = object->get<EBakeSize>("size", EBakeSize::Size2048);

			// see: Projects/Editor/Source/Editor/Components/BakeLight/CBakeLightEditor.cpp
		}
	}
}