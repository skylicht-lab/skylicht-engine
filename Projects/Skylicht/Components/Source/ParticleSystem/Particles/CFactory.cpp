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
#include "CFactory.h"

namespace Skylicht
{
	namespace Particle
	{
		CFactory::CFactory()
		{

		}

		CFactory::~CFactory()
		{
			for (IRenderer *r : m_renderers)
				delete r;

			for (CZone *z : m_zones)
				delete z;

			for (CEmitter *e : m_emitters)
				delete e;

			m_renderers.clear();
			m_zones.clear();
			m_emitters.clear();
		}

		CRandomEmitter* CFactory::createRandomEmitter()
		{
			CRandomEmitter *e = new CRandomEmitter();
			m_emitters.push_back(e);
			return e;
		}

		void CFactory::deleteEmitter(CEmitter *e)
		{
			std::vector<CEmitter*>::iterator i = std::find(m_emitters.begin(), m_emitters.end(), e);
			if (i != m_emitters.end())
			{
				m_emitters.erase(i);
				delete e;
			}
		}

		CQuadRenderer* CFactory::createQuadRenderer()
		{
			CQuadRenderer *r = new CQuadRenderer();
			m_renderers.push_back(r);
			return r;
		}

		void CFactory::deleteRenderer(IRenderer* r)
		{
			std::vector<IRenderer*>::iterator i = std::find(m_renderers.begin(), m_renderers.end(), r);
			if (i != m_renderers.end())
			{
				m_renderers.erase(i);
				delete r;
			}
		}

		CPoint* CFactory::createPointZone()
		{
			CPoint *z = new CPoint();
			m_zones.push_back(z);
			return z;
		}

		void CFactory::deleteZone(CZone *z)
		{
			std::vector<CZone*>::iterator i = std::find(m_zones.begin(), m_zones.end(), z);
			if (i != m_zones.end())
			{
				m_zones.erase(i);
				delete z;
			}
		}
	}
}