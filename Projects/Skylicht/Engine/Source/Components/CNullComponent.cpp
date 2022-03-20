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
#include "CNullComponent.h"
#include "Serializable/CAttributeSerializable.h"

namespace Skylicht
{
	CNullComponent::CNullComponent()
	{
		// this component only save the attribute data
		m_attributes = getIrrlichtDevice()->getFileSystem()->createEmptyAttributes();
	}

	CNullComponent::~CNullComponent()
	{
		m_attributes->drop();
	}

	void CNullComponent::initComponent()
	{

	}

	void CNullComponent::updateComponent()
	{

	}

	CObjectSerializable* CNullComponent::createSerializable()
	{
		CAttributeSerializable* object = new CAttributeSerializable(m_name.c_str());
		object->deserialize(m_attributes);
		return object;
	}

	void CNullComponent::loadSerializable(CObjectSerializable* object)
	{
		// object must be CAttributeSerializable
		object->serialize(m_attributes);
	}
}