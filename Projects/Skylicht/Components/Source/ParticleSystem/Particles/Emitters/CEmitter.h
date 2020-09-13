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

		class CEmitter
		{
		protected:
			int m_lastTank;
			int m_tank;
			float m_flow;
			float m_forceMin;
			float m_forceMax;
			float m_fraction;
			bool m_active;
			bool m_emitFullZone;

			CZone* m_zone;

			EEmitter m_type;
		public:
			CEmitter(EEmitter type);

			virtual ~CEmitter();

			inline CZone* setZone(CZone *z)
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
			}

			inline void setTank(int tank)
			{
				m_tank = tank;
				m_lastTank = tank;
			}

			inline void resetTank()
			{
				setTank(m_lastTank);
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

			virtual u32 updateNumber(float deltaTime);

			void generateVelocity(CParticle& particle, CZone* zone, CGroup *group);

			void emitParticle(CParticle& particle, CZone* zone, CGroup *group);

			virtual void generateVelocity(CParticle& particle, float speed, CZone* zone, CGroup *group) = 0;
		};
	}
}