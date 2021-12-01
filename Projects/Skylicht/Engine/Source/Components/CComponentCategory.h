/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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

#pragma once

#include "Utils/CGameSingleton.h"

namespace Skylicht
{
	// Warning:
	// if this code don't run on SkylichtEditor
	// please register at CEngineEditor.cpp (Projects/Editor/Source/Editor/Components)
#define CATEGORY_COMPONENT(component, name, path) int component##_category = CComponentCategory::createGetInstance()->addToCategory(#component, name, path);

	class CComponentCategory : public CGameSingleton<CComponentCategory>
	{
	public:
		struct SComponent
		{
			std::string ComponentName;
			std::string Name;
			std::string Path;
		};

		struct SCategory
		{
			std::string Name;
			std::vector<SComponent*> Components;
			std::vector<SCategory*> Childs;
			SCategory* Parent;

			SCategory()
			{
				Parent = NULL;
			}

			~SCategory()
			{
				for (SComponent* c : Components)
					delete c;

				for (SCategory* child : Childs)
					delete child;
			}

			SCategory* getChild(const char* name) const
			{
				for (SCategory* child : Childs)
				{
					if (child->Name == name)
						return child;
				}

				return NULL;
			}

			SCategory* addChild(const char* name)
			{
				SCategory* child = new SCategory();
				child->Name = name;
				child->Parent = this;
				Childs.push_back(child);
				std::sort(Childs.begin(), Childs.end(), [](const SCategory* a, const SCategory* b) {return a->Name < b->Name; });
				return child;
			}

			SComponent* add(const char* component, const char* name, const char* path)
			{
				SComponent* s = new SComponent();
				s->ComponentName = component;
				s->Name = name;
				s->Path = path;

				Components.push_back(s);
				return s;
			}
		};

		SCategory* m_category;
		std::vector<SComponent*> m_components;

	public:
		CComponentCategory();

		virtual ~CComponentCategory();

		int addToCategory(const char* componentName, const char* name, const char* path);

		const SCategory* getCategory()
		{
			return m_category;
		}

		const std::vector<SComponent*>& getAllComponents()
		{
			return m_components;
		}
	};
}