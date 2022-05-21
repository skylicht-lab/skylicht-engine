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
#include "CObjectLayer.h"

namespace Skylicht
{
	namespace Editor
	{
		CObjectLayer::CObjectLayer() :
			CObjectSerializable("ObjectLayer")
		{
			Name = "CObjectLayer";

			char name[64];
			for (int i = 0; i < 16; i++)
			{
				sprintf(name, "%d", i);
				autoRelease(new CStringProperty(this, name, ""));
			}

			setName(0, "Default");
			setName(1, "UI");
			setName(2, "Transparent FX");
			setName(3, "Sky");
			setName(4, "Enviroment");
			setName(5, "Collision");
		}

		CObjectLayer::~CObjectLayer()
		{

		}

		const std::string& CObjectLayer::getName(int i)
		{
			CStringProperty* value = dynamic_cast<CStringProperty*>(getPropertyID(i));
			return value->get();
		}

		void CObjectLayer::setName(int i, const char* name)
		{
			CStringProperty* value = dynamic_cast<CStringProperty*>(getPropertyID(i));
			value->set(name);
		}

		void CObjectLayer::saveToFile()
		{
			if (!m_savePath.empty())
				save(m_savePath.c_str());
		}
	}
}