/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CArraySerializable.h"
#include "CValuePropertyTemplate.h"

namespace Skylicht
{
	CArraySerializable::CArraySerializable(const char* name) :
		CObjectSerializable(name)
	{
	}

	CArraySerializable::CArraySerializable(const char* name, CObjectSerializable* parent) :
		CObjectSerializable(name, parent)
	{
	}

	CArraySerializable::~CArraySerializable()
	{

	}

	bool CArraySerializable::resize(int count)
	{
		if (count < 0)
			return false;

		int numElement = getElementCount();
		if (numElement == count)
			return false;

		if (numElement > count)
		{
			// need delete
			for (int i = numElement - 1; i >= count; i--)
			{
				remove(m_value[i]);
			}
		}
		else
		{
			// need grow
			for (int i = numElement; i < count; i++)
			{
				CValueProperty* value = createElement();
				if (!value)
					return false;
			}
		}

		// rename
		char name[32];
		numElement = getElementCount();
		for (int i = 0; i < numElement; i++)
		{
			sprintf(name, "[%d]", i);
			getElement(i)->Name = name;
		}

		return true;
	}
}