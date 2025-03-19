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

		enum EEmitter
		{
			Random,
			Straight,
			Spheric,
			Normal,
			NumOfEmitter,
		};

		struct SBornData
		{
			float Fraction;
			s32 Tank;

			SBornData()
			{
				Fraction = 0.0f;
				Tank = 0;
			}
		};

		class COMPONENT_API CEmitter : public CParticleSerializable
		{
		protected:
			int m_lastTank;
			int m_tank;
			float m_flow;
			float m_lastFlow;
			float m_flowLifeTime;
			float m_forceMin;
			float m_forceMax;
			float m_fraction;
			bool m_active;
			bool m_emitFullZone;

			float m_delay;
			float m_waitDelay;
			float m_lifeTime;

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

			virtual bool haveDirection()
			{
				return false;
			}

			inline CZone* setZone(CZone* z)
			{
				m_zone = z;
				return m_zone;
			}

			inline CZone* getZone()
			{
				return m_zone;
			}

			inline void stop()
			{
				m_tank = 0;
				m_flow = 0;
				m_lifeTime = 0.0f;
			}

			inline void setTank(int tank)
			{
				m_tank = tank;
				m_lastTank = tank;
			}

			inline void resetTank()
			{
				setTank(m_lastTank);
				setFlow(m_lastFlow);
				m_waitDelay = m_delay;
				m_lifeTime = 0.0f;
			}

			inline int getTank()
			{
				return m_tank;
			}

			inline void setFlow(float flow)
			{
				m_flow = flow;
				if (m_flow <= 0)
					m_flow = 0;
				m_lastFlow = flow;
			}

			inline float getFlow()
			{
				return m_flow;
			}

			inline void setActive(bool b)
			{
				m_active = b;
			}

			inline bool isActive()
			{
				return m_active;
			}

			inline void setDelay(float timeSecond)
			{
				m_delay = timeSecond;
				m_waitDelay = timeSecond;
			}

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

			inline float getForceMin()
			{
				return m_forceMin;
			}

			inline float getForceMax()
			{
				return m_forceMax;
			}

			inline void setEmitFullZone(bool b)
			{
				m_emitFullZone = b;
			}

			inline bool isEmitFullZone()
			{
				return m_emitFullZone;
			}

			inline EEmitter getType()
			{
				return m_type;
			}

			const wchar_t* getName();

			virtual u32 updateNumber(float deltaTime);

			virtual void setBornData(SBornData& data);

			inline u32 updateBornData(u32 index, float deltaTime)
			{
				return updateBornData(m_bornData[index], deltaTime);
			}

			virtual u32 updateBornData(SBornData& data, float deltaTime);

			void generateVelocity(CParticle& particle, CZone* zone, CGroup* group);

			void emitParticle(CParticle& particle, CZone* zone, CGroup* group);

			virtual void generateVelocity(CParticle& particle, float speed, CZone* zone, CGroup* group) = 0;

			u32 addBornData();

			void swapBornData(int index1, int index2);

			void deleteBornData();

			DECLARE_GETTYPENAME(CEmitter)
		};
	}
}