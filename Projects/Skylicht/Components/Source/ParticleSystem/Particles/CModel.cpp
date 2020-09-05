/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CModel.h"
#include "ParticleSystem/Particles/Zones/CZone.h"

namespace Skylicht
{
	namespace Particle
	{
		CModel::CModel(EParticleParams type) :
			m_type(type),
			m_start1(0),
			m_start2(0),
			m_end1(0),
			m_end2(0),
			m_haveStart(false),
			m_haveEnd(false),
			m_interpolator(NULL)
		{

		}

		CModel::~CModel()
		{
			if (m_interpolator != NULL)
				delete m_interpolator;
		}

		float CModel::getRandomStart()
		{
			return random(m_start1, m_start2);
		}

		float CModel::getRandomEnd()
		{
			return random(m_end1, m_end2);
		}
	}
}