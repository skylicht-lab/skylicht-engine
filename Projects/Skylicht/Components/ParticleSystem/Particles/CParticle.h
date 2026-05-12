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

#include "Entity/IEntityData.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @enum EParticleParams
		 * @ingroup ParticleSystem
		 * @brief IDs for particle parameters that can be animated over time.
		 */
		enum EParticleParams
		{
			Scale = 0,
			ScaleX,
			ScaleY,
			ScaleZ,
			RotateX,
			RotateY,
			RotateZ,
			ColorR,
			ColorG,
			ColorB,
			ColorA,
			Mass,
			FrameIndex,
			RotateSpeedX,
			RotateSpeedY,
			RotateSpeedZ,
			NumParams
		};

		/**
		 * @class CParticle
		 * @ingroup ParticleSystem
		 * @brief Individual particle data structure.
		 */
		class COMPONENT_API CParticle
		{
		public:
			/** @brief Global index in the group's particle array. */
			u32 Index;
			/** @brief Index of the parent particle in a parent group (for sub-emitters). */
			s32 ParentIndex;

			/** @brief If true, this particle never dies from age. */
			bool Immortal;

			/** @brief Current values of animated parameters. @see EParticleParams */
			float Params[NumParams];
			/** @brief Initial values of animated parameters. */
			float StartValue[NumParams];
			/** @brief Targeted end values of animated parameters. */
			float EndValue[NumParams];

			/** @brief Time since birth in seconds. */
			float Age;
			/** @brief Remaining time to live in seconds. */
			float Life;
			/** @brief Total expected lifespan in seconds. */
			float LifeTime;

			/** @brief Optimization flag: whether this particle has active rotation logic. */
			bool HaveRotate;

			/** @brief Current world position. */
			core::vector3df Position;
			/** @brief Current world rotation (Euler). */
			core::vector3df Rotation;
			/** @brief Current movement velocity. */
			core::vector3df Velocity;

			/** @brief Position in the previous frame. */
			core::vector3df LastPosition;

			/** @brief Initial direction for sub-emitters if velocity is zero. */
			core::vector3df SubEmitterDirection;
			/** @brief Optional user-defined data. */
			void* UserData;

		public:
			CParticle(u32 index);

			virtual ~CParticle();

			/** @brief Swaps all data with another particle instance. */
			void swap(CParticle& p);
		};
	}
}