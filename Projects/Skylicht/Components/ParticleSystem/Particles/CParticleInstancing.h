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

namespace Skylicht
{
	namespace Particle
	{
		struct SParticleInstance
		{
			video::SVec4 Pos;
			SColor Color;
			core::vector2df UVScale;
			core::vector2df UVOffset;
			video::SVec4 Size;
			video::SVec4 Rotation;
			video::SVec4 Velocity;

			bool operator==(const SParticleInstance& other) const
			{
				return Pos == other.Pos
					&& Size == other.Size
					&& Rotation == other.Rotation
					&& Color == other.Color
					&& UVScale == other.UVScale
					&& UVOffset == other.UVOffset
					&& Velocity == other.Velocity;
			}
		};

		class COMPONENT_API CParticleInstancing
		{
		protected:
			IMeshBuffer* m_meshBuffer;
			CVertexBuffer<SParticleInstance>* m_instanceBuffer;

		public:
			CParticleInstancing();

			virtual ~CParticleInstancing();

			inline IMeshBuffer* getMeshBuffer()
			{
				return m_meshBuffer;
			}

			inline CVertexBuffer<SParticleInstance>* getInstanceBuffer()
			{
				return m_instanceBuffer;
			}
		};
	}
}