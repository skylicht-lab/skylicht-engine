/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include "Utils/CGameSingleton.h"

namespace Skylicht
{

	class CAccelerometer : public CGameSingleton < CAccelerometer >
	{
	protected:
		bool				m_support;
		bool				m_enable;

		core::vector3df		m_vector;
		float m_rotX;
		float m_rotY;

	public:
		CAccelerometer();
		virtual ~CAccelerometer();

		void setSupport(bool b)
		{
			m_support = b;
		}

		bool isSupport()
		{
			return m_support;
		}

		void setEnable(bool b)
		{
			m_enable = b;
		}

		bool isEnable()
		{
			return m_enable;
		}

		const core::vector3df& getVector()
		{
			return m_vector;
		}

		void setVector(float x, float y, float z);

		void update();

		inline float getRotX()
		{
			return m_rotX;
		}

		inline float getRotY()
		{
			return m_rotY;
		}

	};

}

#endif