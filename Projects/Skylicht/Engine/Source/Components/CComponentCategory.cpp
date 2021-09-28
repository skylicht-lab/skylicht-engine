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

#include "pch.h"
#include "CComponentCategory.h"
#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CComponentCategory::CComponentCategory()
	{
		m_category = new SCategory();
		m_category->Name = "Components";
	}

	CComponentCategory::~CComponentCategory()
	{
		delete m_category;
	}

	int CComponentCategory::addToCategory(const char* componentName, const char* name, const char* path)
	{
		std::vector<std::string> folder;
		CStringImp::splitString(path, "/", folder);

		SCategory* current = m_category;

		for (std::string& s : folder)
		{
			SCategory* child = current->getChild(s.c_str());
			if (child == NULL)
				child = current->addChild(s.c_str());

			current = child;
		}

		SComponent* comp = current->add(componentName, name, path);
		m_components.push_back(comp);

		return (int)m_components.size();
	}
}