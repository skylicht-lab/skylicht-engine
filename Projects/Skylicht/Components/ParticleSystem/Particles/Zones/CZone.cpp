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
#include "CZone.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Particle
	{
		// use local random
		s32 seed = 0x0f0f0f0f;
		const s32 m = 2147483399;	// a non-Mersenne prime
		const s32 a = 40692;		// another spectral success story
		const s32 q = m / a;
		const s32 r = m % a;		// again less than q
		const s32 rMax = m - 1;

		s32 particle_rand()
		{
			// (a*seed)%m with Schrage's method
			seed = a * (seed % q) - r * (seed / q);
			if (seed < 0)
				seed += m;

			return seed;
		}

		f32 particle_frand()
		{
			return particle_rand() * (1.f / rMax);
		}

		s32 particle_rand_max()
		{
			return rMax;
		}

		void random_reset(s32 value)
		{
			seed = value;
		}

		int random(int from, int to)
		{
			s32 r = particle_rand() % (to - from);
			return from + r;
		}

		float random(float from, float to)
		{
			return from + (to - from) * particle_frand();
		}

		CZone::CZone(EZone type) :
			m_type(type)
		{

		}

		CZone::~CZone()
		{

		}

		const wchar_t* g_zoneName[] =
		{
			L"Point",
			L"Sphere",
			L"AABox",
			L"Cylinder",
			L"Line",
			L"PolyLine",
			L"Ring",
			L"NumOfZone"
		};

		const wchar_t* CZone::getName()
		{
			return g_zoneName[(int)m_type];
		}

		void CZone::normalizeOrRandomize(core::vector3df& v)
		{
			while (v.getLengthSQ() == 0.0f)
			{
				v.X = random(-1.0f, 1.0f);
				v.Y = random(-1.0f, 1.0f);
				v.Z = random(-1.0f, 1.0f);
			}

			v.normalize();
		}

		CObjectSerializable* CZone::createSerializable()
		{
			CObjectSerializable* object = CParticleSerializable::createSerializable();

			CStringProperty* name = new CStringProperty(object, "name", CStringImp::convertUnicodeToUTF8(getName()).c_str());
			name->setHidden(true);
			object->autoRelease(name);

			return object;
		}

		void CZone::loadSerializable(CObjectSerializable* object)
		{
			CParticleSerializable::loadSerializable(object);
		}
	}
}