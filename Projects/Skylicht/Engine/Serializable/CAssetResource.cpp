/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CAssetResource.h"
#include "Utils/CRandomID.h"

namespace Skylicht
{
	CAssetResource::CAssetResource(const char* name) :
		CObjectSerializable(name),
		GUID(this, "guid", CRandomID::generate().c_str()),
		GenerateNewId(true)
	{
		OtherName.push_back("CAssetResource");
		GUID.setHidden(true);
	}

	CAssetResource::~CAssetResource()
	{

	}

	void CAssetResource::deserialize(io::IAttributes* io)
	{
		std::string oldId = GUID.get();

		CObjectSerializable::deserialize(io);

		if (GUID.get().empty())
		{
			GUID.set(CRandomID::generate());
			GenerateNewId = true;
		}
		else if (GUID.get() != oldId)
		{
			GenerateNewId = false;
		}
	}

	bool CAssetResource::save(const char* file)
	{
		bool ret = CObjectSerializable::save(file);
		if (ret)
			GenerateNewId = false;
		return ret;
	}
}