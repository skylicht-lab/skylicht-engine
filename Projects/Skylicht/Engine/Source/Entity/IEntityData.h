/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#include "Utils/CActivator.h"
#include "Utils/CMemoryStream.h"
#include "Entity/CEntityDataTypeManager.h"

namespace Skylicht
{
	class IMeshExporter;
	class IMeshImporter;
	class CEntity;

	class SKYLICHT_API IEntityData : public IActivatorObject
	{
	public:
		int EntityIndex;
		CEntity* Entity;
	public:
		IEntityData() :
			EntityIndex(-1),
			Entity(NULL)
		{

		}

		virtual ~IEntityData()
		{

		}

		virtual bool serializable(CMemoryStream* stream)
		{
			return false;
		}

		virtual bool deserializable(CMemoryStream* stream, int version)
		{
			return false;
		}

		virtual std::string getTypeName()
		{
			return std::string("IEntityData");
		}
	};
}