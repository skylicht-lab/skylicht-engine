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
		 * @brief Internal data for managing particle birth rates for sub-emitters.
		 * @details Keeps track of individual tanks and flows when a sub-group spawns particles from parent particles.
		 */
		struct SBornData
		{
			/** @brief Accumulated life time of the emitter logic. */
			float LifeTime;
			/** @brief Maximum duration for the flow logic. */
			float FlowLifeTime;
			/** @brief Initial delay before birth starts. */
			float WaitDelay;
			/** @brief Fraction for sub-frame birth calculations. */
			float Fraction;
			/** @brief Remaining particles to be spawned from the "tank". */
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
		 * They can operate in "Flow" mode (continuous) or "Tank" mode (limited count).
		 */
		class COMPONENT_API CEmitter : public CParticleSerializable
		{
		protected:
			int m_lastTank;
			int m_defaultTank;
			/** @brief Current particles remaining in the tank. */
			int m_tank;
			/** @brief Current birth rate (particles per second). */
			float m_flow;
			float m_lastFlow;
			float m_defaultFlow;
			/** @brief Duration of the flow logic before it stops. */
			float m_flowLifeTime;
			/** @brief Minimum magnitude of initial velocity. */
			float m_forceMin;
			/** @brief Maximum magnitude of initial velocity. */
			float m_forceMax;
			float m_fraction;
			/** @brief Active status. */
			bool m_active;
			/** @brief Whether to spawn particles randomly within the zone volume. */
			bool m_emitFullZone;

			/** @brief Configured delay. */
			float m_delay;
			/** @brief Current waiting time for delay. */
			float m_waitDelay;
			/** @brief Total time elapsed since start. */
			float m_lifeTime;

			/** @brief Current reset timer. */
			float m_reset;
			/** @brief Min interval for auto-resetting the tank. */
			float m_resetIntervalMin;
			/** @brief Max interval for auto-resetting the tank. */
			float m_resetIntervalMax;

			/** @brief The spawn zone assigned to this emitter. */
			CZone* m_zone;

			/** @brief Type of the emitter. */
			EEmitter m_type;

		protected:

			/** @brief Internal: birth data for sub-emitters. */
			core::array<SBornData> m_bornData;

		public:
			CEmitter(EEmitter type);

			virtual ~CEmitter();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Returns true if this emitter has a primary direction. */
			virtual bool haveDirection()
			{
				return false;
			}

			/** @brief Sets the spawn zone. */
			inline CZone* setZone(CZone* z)
			{
				m_zone = z;
				return m_zone;
			}

			/** @brief Gets the current spawn zone. */
			inline CZone* getZone()
			{
				return m_zone;
			}

			/** @brief Stops the emitter immediately. */
			inline void stop()
			{
				m_tank = 0;
				m_flow = 0;
				m_lifeTime = 0.0f;
				m_reset = 0.0f;
			}

			/** @brief Sets the tank capacity and current value. */
			inline void setTank(int tank)
			{
				m_tank = tank;
				m_lastTank = tank;
			}

			/** @brief Resets the tank and flow logic. */
			void resetTank();

			/** @brief Gets current tank value. */
			inline int getTank()
			{
				return m_tank;
			}

			/** @brief Gets last set tank value. */
			inline int getLastTank()
			{
				return m_lastTank;
			}

			/** @brief Updates the persistent tank value without immediate reset. */
			inline void setTankValue(int tank)
			{
				m_lastTank = tank;
			}

			/** @brief Gets default tank from serializable data. */
			inline int getDefaultTank()
			{
				return m_defaultTank;
			}

			/** @brief Gets default flow from serializable data. */
			inline float getDefaultFlow()
			{
				return m_defaultFlow;
			}

			/** @brief Sets continuous birth rate (particles per second). */
			inline void setFlow(float flow)
			{
				m_flow = flow;
				if (m_flow <= 0)
					m_flow = 0;
				m_lastFlow = flow;
			}

			/** @brief Gets current flow rate. */
			inline float getFlow()
			{
				return m_flow;
			}

			/** @brief Enables or disables the emitter. */
			inline void setActive(bool b)
			{
				m_active = b;
			}

			/** @brief Checks if emitter is active. */
			inline bool isActive()
			{
				return m_active;
			}

			/** @brief Sets start delay in seconds. */
			inline void setDelay(float timeSecond)
			{
				m_delay = timeSecond;
				m_waitDelay = timeSecond;
			}

			/** @brief Sets initial velocity force range. */
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

			/** @brief Gets min force. */
			inline float getForceMin()
			{
				return m_forceMin;
			}

			/** @brief Gets max force. */
			inline float getForceMax()
			{
				return m_forceMax;
			}

			/** @brief Sets interval for automatic tank resets. */
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

			/** @brief Sets volume spawn mode. */
			inline void setEmitFullZone(bool b)
			{
				m_emitFullZone = b;
			}

			/** @brief Checks volume spawn mode. */
			inline bool isEmitFullZone()
			{
				return m_emitFullZone;
			}

			/** @brief Gets emitter type. */
			inline EEmitter getType()
			{
				return m_type;
			}

			/** @brief Gets display name. */
			const wchar_t* getName();

			/** @brief Calculates how many particles should be born this frame. */
			virtual u32 updateNumber(float deltaTime);

			/** @brief Initializes birth data for sub-emitters. */
			virtual void setBornData(SBornData& data);

			/** @brief Internal: updates birth calculation for a specific sub-group particle index. */
			inline u32 updateBornData(u32 index, float deltaTime)
			{
				return updateBornData(m_bornData[index], deltaTime);
			}

			/** @brief Internal: core logic for sub-emitter birth calculation. */
			virtual u32 updateBornData(SBornData& data, float deltaTime);

			/** @brief Clears sub-emitter birth records. */
			inline void clearBornData()
			{
				m_bornData.clear();
			}

			/** @brief Generates initial velocity for a particle. */
			void generateVelocity(CParticle& particle, CZone* zone, CGroup* group);

			/** @brief Full emission logic: generates position and velocity. */
			void emitParticle(CParticle& particle, CZone* zone, CGroup* group);

			/** @brief Implementation-specific velocity generation. */
			virtual void generateVelocity(CParticle& particle, float speed, CZone* zone, CGroup* group) = 0;

			/** @brief Adds a new sub-emitter record. */
			u32 addBornData();

			/** @brief Swaps sub-emitter records (for array reordering). */
			void swapBornData(int index1, int index2);

			/** @brief Removes a sub-emitter record. */
			void deleteBornData();

			DECLARE_GETTYPENAME(CEmitter)
		};
	}
}