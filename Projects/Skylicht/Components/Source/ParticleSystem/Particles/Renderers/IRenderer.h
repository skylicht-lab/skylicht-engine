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

#pragma once

#include "pch.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	namespace Particle
	{
		enum ERenderer
		{
			Quad,
			BillboardAddtive
		};

		class IRenderer
		{
		protected:
			ERenderer m_type;
			CMaterial *m_material;

			bool m_useInstancing;
			bool m_emission;

		public:
			float SizeX;
			float SizeY;
			float SizeZ;

		public:
			IRenderer(ERenderer type) :
				m_type(type),
				m_material(NULL),
				SizeX(1.0f),
				SizeY(1.0f),
				SizeZ(1.0f),
				m_useInstancing(true),
				m_emission(false)
			{

			}

			virtual ~IRenderer()
			{

			}

			inline ERenderer getType()
			{
				return m_type;
			}

			CMaterial* getMaterial()
			{
				return m_material;
			}

			inline void setEmission(bool b)
			{
				m_emission = b;
			}

			inline bool isEmission()
			{
				return m_emission;
			}

			bool useInstancing()
			{
				return m_useInstancing;
			}

			virtual void getParticleBuffer(IMeshBuffer *buffer) = 0;

			virtual u32 getTotalFrames()
			{
				return 1;
			}
		};
	}
}