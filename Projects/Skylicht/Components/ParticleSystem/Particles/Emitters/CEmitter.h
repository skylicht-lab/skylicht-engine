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

#include "ParticleSystem/Particles/CParticleSerializable.h"

namespace Skylicht
{
	namespace Particle
	{
		class CZone;
		class CParticle;
		class CGroup;

		/**
		 * @enum EEmitter
		 * @ingroup ParticleSystem
		 * @brief Available particle emitter types.
		 */
		enum EEmitter
		{
			Random,
			Straight,
			Spheric,
			Normal,
			NumOfEmitter,
		};

		/**
		 * @struct SBornData
		 * @ingroup ParticleSystem
		 * @brief Internal data for managing particle birth rates.
		 */
		struct SBornData
		{
			float LifeTime;
			float FlowLifeTime;
			float WaitDelay;
			float Fraction;
			s32 Tank;

			SBornData()
			{
				LifeTime = 0.0f;
				FlowLifeTime = 0.0f;
				WaitDelay = 0.0f;
				Fraction = 0.0f;
				Tank = 0;
			}
		};

		/**
		 * @class CEmitter
		 * @ingroup ParticleSystem
		 * @brief Base class for particle emitters.
		 * @details Emitters control how many particles are born per second (Flow) and their initial velocity.
		 */
		class COMPONENT_API CEmitter : public CParticleSerializable
		{
		protected:
			int m_lastTank;
			int m_defaultTank;
			int m_tank;
			float m_flow;
			float m_lastFlow;
			float m_defaultFlow;
			float m_flowLifeTime;
			float m_forceMin;
			float m_forceMax;
			float m_fraction;
			bool m_active;
			bool m_emitFullZone;

			float m_delay;
			float m_waitDelay;
			float m_lifeTime;

			float m_reset;
			float m_resetIntervalMin;
			float m_resetIntervalMax;

			CZone* m_zone;

			EEmitter m_type;

		protected:

			// sub emitter
			core::array<SBornData> m_bornData;

		public:
			CEmitter(EEmitter type);

			virtual ~CEmitter();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Check if this emitter has a primary emission direction.
			 * @return True if directional.
			 */
			virtual bool haveDirection()
			{
				return false;
			}

			/**
			 * @brief Set the spawn zone for this emitter.
			 * @param z Pointer to CZone.
			 * @return Pointer to the set zone.
			 */
			inline CZone* setZone(CZone* z)
			{
				m_zone = z;
				return m_zone;
			}

			/**
			 * @brief Get the spawn zone.
			 * @return Pointer to CZone.
			 */
			inline CZone* getZone()
			{
				return m_zone;
			}

			/**
			 * @brief Stop particle emission.
			 */
			inline void stop()
			{
				m_tank = 0;
				m_flow = 0;
				m_lifeTime = 0.0f;
				m_reset = 0.0f;
			}

			/**
			 * @brief Set the total number of particles that can be emitted.
			 * @param tank Particle count. Use -1 for infinite.
			 */
			inline void setTank(int tank)
			{
				m_tank = tank;
				m_lastTank = tank;
			}

			/**
			 * @brief Reset the tank and flow values to their defaults.
			 */
			void resetTank();

			/**
			 * @brief Get remaining particles in the tank.
			 * @return Particle count.
			 */
			inline int getTank()
			{
				return m_tank;
			}

			/**
			 * @brief Set the birth rate of particles.
			 * @param flow Particles per second.
			 */
			inline void setFlow(float flow)
			{
				m_flow = flow;
				if (m_flow <= 0)
					m_flow = 0;
				m_lastFlow = flow;
			}

			/**
			 * @brief Get current birth rate.
			 * @return Particles per second.
			 */
			inline float getFlow()
			{
				return m_flow;
			}

			/**
			 * @brief Enable or disable the emitter.
			 * @param b True to activate.
			 */
			inline void setActive(bool b)
			{
				m_active = b;
			}

			/**
			 * @brief Check if the emitter is active.
			 * @return True if active.
			 */
			inline bool isActive()
			{
				return m_active;
			}

			/**
			 * @brief Set initial delay before emission starts.
			 * @param timeSecond Delay in seconds.
			 */
			inline void setDelay(float timeSecond)
			{
				m_delay = timeSecond;
				m_waitDelay = timeSecond;
			}

			/**
			 * @brief Set the range of initial speeds for particles.
			 * @param min Minimum speed.
			 * @param max Maximum speed.
			 */
			inline void setForce(float min, float max)
			{
				if (min > max)
				{
					m_forceMin = max;
					m_forceMax = min;
				}
				else
				{
					m_forceMin = min;
					m_forceMax = max;
				}
			}

			/**
			 * @brief Set an interval to automatically reset the tank (burst mode).
			 * @param min Minimum interval in seconds.
			 * @param max Maximum interval in seconds.
			 */
			inline void setResetTankInterval(float min, float max)
			{
				if (min > max)
				{
					m_resetIntervalMin = max;
					m_resetIntervalMax = min;
				}
				else
				{
					m_resetIntervalMin = min;
					m_resetIntervalMax = max;
				}
			}

			/**
			 * @brief Whether to spawn particles within the entire zone or only on its surface.
			 * @param b True to spawn in full volume.
			 */
			inline void setEmitFullZone(bool b)
			{
				m_emitFullZone = b;
			}

			/**
			 * @brief Get the emitter type ID.
			 * @return EEmitter enum.
			 */
			inline EEmitter getType()
			{
				return m_type;
			}

			const wchar_t* getName();

			/**
			 * @brief Calculate the number of particles to spawn in the current frame.
			 * @param deltaTime Time step in milliseconds.
			 * @return Spawn count.
			 */
			virtual u32 updateNumber(float deltaTime);

			/**
			 * @brief Internal: Generate velocity for a single particle based on speed.
			 * @param particle Target particle.
			 * @param speed Calculated speed value.
			 * @param zone Spawn zone.
			 * @param group Parent group.
			 */
			virtual void generateVelocity(CParticle& particle, float speed, CZone* zone, CGroup* group) = 0;

			DECLARE_GETTYPENAME(CEmitter)
		};
	}
}