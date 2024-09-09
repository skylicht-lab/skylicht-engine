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

#pragma once

#include "Components/CComponentSystem.h"
#include "CSkySunData.h"
#include "CSkySunRenderer.h"

namespace Skylicht
{
	class COMPONENT_API CSkySun : public CComponentSystem
	{
	protected:
		CSkySunData* m_skySunData;

		bool m_useDirectionLight;

		float m_skyIntensity;
		float m_sunSize;

		SColor m_atmosphericColor;
		float m_atmosphericIntensity;

		SColor m_sunColor;
		float m_sunIntensity;

		SColor m_glare1Color;
		float m_glare1Intensity;

		SColor m_glare2Color;
		float m_glare2Intensity;

		bool m_changed;

		CMaterial::SUniformValue* m_uniformDirection;

	public:
		CSkySun();

		virtual ~CSkySun();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CSkySun)

	protected:

		void getUniformValue(const SColor& c, float intensity, float* value);

	};
}