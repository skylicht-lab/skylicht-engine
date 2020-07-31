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
#include "CSH9.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CSH9::CSH9()
		{

		}

		CSH9::~CSH9()
		{

		}

		CSH9::CSH9(const core::vector3df *sh)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = sh[i];
		}

		CSH9::CSH9(const CSH9& sh)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = sh.m_sh[i];
		}

		void CSH9::zero()
		{
			for (int i = 0; i < 9; i++)
				m_sh[i].set(0.0f, 0.0f, 0.0f);
		}

		CSH9 CSH9::operator-() const
		{
			CSH9 r(m_sh);
			for (int i = 0; i < 9; i++)
				r.m_sh[i] = -r.m_sh[i];
			return r;
		}

		CSH9& CSH9::operator=(const CSH9& other)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = other.m_sh[i];
			return *this;
		}

		CSH9 CSH9::operator+(const CSH9& other) const
		{
			CSH9 r(m_sh);
			for (int i = 0; i < 9; i++)
				r.m_sh[i] = r.m_sh[i] + other.m_sh[i];
			return r;
		}

		CSH9& CSH9::operator+=(const CSH9& other)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = m_sh[i] + other.m_sh[i];
			return *this;
		}
		CSH9 CSH9::operator-(const CSH9& other) const
		{
			CSH9 r(m_sh);
			for (int i = 0; i < 9; i++)
				r.m_sh[i] = r.m_sh[i] - other.m_sh[i];
			return r;
		}

		CSH9& CSH9::operator-=(const CSH9& other)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = m_sh[i] - other.m_sh[i];
			return *this;
		}

		CSH9 CSH9::operator*(const CSH9& other) const
		{
			CSH9 r(m_sh);
			for (int i = 0; i < 9; i++)
				r.m_sh[i] = r.m_sh[i] * other.m_sh[i];
			return r;
		}

		CSH9& CSH9::operator*=(const CSH9& other)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = m_sh[i] * other.m_sh[i];
			return *this;
		}

		CSH9 CSH9::operator*(const float v) const
		{
			CSH9 r(m_sh);
			for (int i = 0; i < 9; i++)
				r.m_sh[i] = r.m_sh[i] * v;
			return r;
		}

		CSH9& CSH9::operator*=(const float v)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = m_sh[i] * v;
			return *this;
		}

		CSH9 CSH9::operator/(const CSH9& other) const
		{
			CSH9 r(m_sh);
			for (int i = 0; i < 9; i++)
				r.m_sh[i] = r.m_sh[i] / other.m_sh[i];
			return r;
		}

		CSH9& CSH9::operator/=(const CSH9& other)
		{
			for (int i = 0; i < 9; i++)
				m_sh[i] = m_sh[i] / other.m_sh[i];
			return *this;
		}

		CSH9 CSH9::operator/(const float v) const
		{
			float inv = 1.0f / v;
			CSH9 r(m_sh);
			for (int i = 0; i < 9; i++)
				r.m_sh[i] = r.m_sh[i] * inv;
			return r;
		}

		CSH9& CSH9::operator/=(const float v)
		{
			float inv = 1.0f / v;
			for (int i = 0; i < 9; i++)
				m_sh[i] = m_sh[i] * inv;
			return *this;
		}

		core::vector3df CSH9::dotProduct(const CSH9& other)
		{
			core::vector3df r;
			for (int i = 0; i < 9; i++)
				r = r + m_sh[i] * other.m_sh[i];
			return r;
		}

		void CSH9::projectAddOntoSH(const core::vector3df& n, const core::vector3df& color, float A0, float A1, float A2)
		{
			// Band 0
			m_sh[0] = m_sh[0] + 0.282095f * color * A0;

			// Band 1
			m_sh[1] = m_sh[1] + 0.488603f * n.Y * color * A1;
			m_sh[2] = m_sh[2] + 0.488603f * n.Z * color * A1;
			m_sh[3] = m_sh[3] + 0.488603f * n.X * color * A1;

			// Band 2
			m_sh[4] = m_sh[4] + 1.092548f * n.X * n.Y * color * A2;
			m_sh[5] = m_sh[5] + 1.092548f * n.Y * n.Z * color * A2;
			m_sh[6] = m_sh[6] + 0.315392f * (3.0f * n.Z * n.Z - 1.0f) * color * A2;

			m_sh[7] = m_sh[7] + 1.092548f * n.X * n.Z * color * A2;
			m_sh[8] = m_sh[8] + 0.546274f * (n.X * n.X - n.Y * n.Y) * color * A2;
		}

		void CSH9::projectOntoSH(const core::vector3df& n, const core::vector3df& color, float A0, float A1, float A2)
		{
			// https://github.com/TheRealMJP/LowResRendering/blob/master/SampleFramework11/v1.01/Graphics/SH.cpp
			// Band 0
			m_sh[0] = 0.282095f * color * A0;

			// Band 1
			m_sh[1] = 0.488603f * n.Y * color * A1;
			m_sh[2] = 0.488603f * n.Z * color * A1;
			m_sh[3] = 0.488603f * n.X * color * A1;

			// Band 2
			m_sh[4] = 1.092548f * n.X * n.Y * color * A2;
			m_sh[5] = 1.092548f * n.Y * n.Z * color * A2;
			m_sh[6] = 0.315392f * (3.0f * n.Z * n.Z - 1.0f) * color * A2;

			m_sh[7] = 1.092548f * n.X * n.Z * color * A2;
			m_sh[8] = 0.546274f * (n.X * n.X - n.Y * n.Y) * color * A2;
		}

		void CSH9::getSH(const core::vector3df& n, core::vector3df& color)
		{
			// EvalSH9
			// https://github.com/TheRealMJP/BakingLab/blob/master/SampleFramework11/v1.02/Shaders/SH.hlsl
			CSH9 dirSH;
			dirSH.projectOntoSH(n, core::vector3df(1.0f, 1.0f, 1.0f));

			color = dirSH.dotProduct(*this);
		}

		void CSH9::getSHIrradiance(const core::vector3df& n, core::vector3df& color)
		{
			// Cosine kernel for SH
			const float CosineA0 = core::PI;
			const float CosineA1 = (2.0f * core::PI) / 3.0f;
			const float CosineA2 = core::PI / 4.0f;

			CSH9 dirSH;
			dirSH.projectOntoSH(n, core::vector3df(1.0f, 1.0f, 1.0f), CosineA0, CosineA1, CosineA2);

			color = dirSH.dotProduct(*this);
		}

		void CSH9::getSHLMIrradiance(const core::vector3df& n, core::vector3df& color)
		{
			const float CosineA0 = core::PI;
			const float CosineA1 = (2.0f * core::PI) / 3.0f;

			core::vector3df sh[4];
			core::vector3df c(1.0f, 1.0f, 1.0f);

			// Band 0
			sh[0] = 0.282095f * c * CosineA0;

			// Band 1
			sh[1] = 0.488603f * n.Y * c * CosineA1;
			sh[2] = 0.488603f * n.Z * c * CosineA1;
			sh[3] = 0.488603f * n.X * c * CosineA1;

			color.set(0.0f, 0.0f, 0.0f);
			for (int i = 0; i < 4; i++)
				color = color + m_sh[i] * sh[i];
		}

		void CSH9::convolveWithCosineKernel()
		{
			const float CosineA0 = core::PI;
			const float CosineA1 = (2.0f * core::PI) / 3.0f;
			const float CosineA2 = core::PI / 4.0f;

			m_sh[0] *= CosineA0;

			m_sh[1] *= CosineA1;
			m_sh[2] *= CosineA1;
			m_sh[3] *= CosineA1;

			m_sh[4] *= CosineA2;
			m_sh[5] *= CosineA2;
			m_sh[6] *= CosineA2;
			m_sh[7] *= CosineA2;
			m_sh[8] *= CosineA2;
		}
	}
}