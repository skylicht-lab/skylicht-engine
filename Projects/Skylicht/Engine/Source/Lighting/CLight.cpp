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

#include "pch.h"
#include "CLight.h"

namespace Skylicht
{
	CLight::CLight() :
		m_castShadow(false),
		m_color(1.0f, 1.0f, 1.0f, 1.0f),
		m_spotCutoff(core::PI / 4.0f),
		m_intensity(1.0f),
		m_bakeBounce(1)
	{
		setRadius(3.0f);
	}

	CLight::~CLight()
	{

	}

	CObjectSerializable* CLight::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();
		object->autoRelease(new CUIntProperty(object, "bakeBounce", m_bakeBounce, 4));
		object->autoRelease(new CBoolProperty(object, "castShadow", m_castShadow));
		object->autoRelease(new CColorProperty(object, "color", m_color.toSColor()));
		object->autoRelease(new CFloatProperty(object, "intensity", m_intensity, 0.0f, 10.0f));
		return object;
	}

	void CLight::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);
		m_bakeBounce = object->get<u32>("bakeBounce", 1);
		m_castShadow = object->get<bool>("castShadow", false);
		m_color = object->get<SColor>("color", SColor(255, 255, 255, 255));
		m_intensity = object->get<float>("intensity", 1.0f);
	}
}