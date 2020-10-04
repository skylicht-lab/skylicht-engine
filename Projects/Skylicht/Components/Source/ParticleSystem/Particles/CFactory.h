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

#include "Renderers/IRenderer.h"
#include "Zones/CZone.h"
#include "Emitters/CEmitter.h"

#include "Renderers/CQuadRenderer.h"

#include "Zones/CPoint.h"
#include "Zones/CSphere.h"
#include "Zones/CAABox.h"
#include "Zones/CCylinder.h"
#include "Zones/CLine.h"
#include "Zones/CPolyLine.h"
#include "Zones/CRing.h"

#include "Emitters/CRandomEmitter.h"
#include "Emitters/CStraightEmitter.h"
#include "Emitters/CSphericEmitter.h"
#include "Emitters/CNormalEmitter.h"

#include "Systems/CVortexSystem.h"

namespace Skylicht
{
	namespace Particle
	{
		class CFactory
		{
		protected:
			std::vector<IRenderer*> m_renderers;
			std::vector<CZone*> m_zones;
			std::vector<CEmitter*> m_emitters;

		public:
			CFactory();

			virtual ~CFactory();

			CRandomEmitter* createRandomEmitter();

			CStraightEmitter* createStraightEmitter(const core::vector3df& direction);

			CSphericEmitter* createSphericEmitter(const core::vector3df& direction, float angleA, float angleB);

			CNormalEmitter* createNormalEmitter(bool inverted);

			void deleteEmitter(CEmitter *e);

			CQuadRenderer* createQuadRenderer();

			void deleteRenderer(IRenderer* r);

			CPoint* createPointZone();

			CPoint* createPointZone(const core::vector3df& pos);

			CSphere* createSphereZone(const core::vector3df& pos, float radius);

			CAABox* createAABoxZone(const core::vector3df& pos, const core::vector3df& dimension);

			CCylinder* createCylinderZone(const core::vector3df& pos, const core::vector3df& direction, float radius, float length);

			CLine* createLineZone(const core::vector3df& p1, const core::vector3df& p2);

			CPolyLine* createPolyLineZone(const core::array<core::vector3df>& points);

			CRing* createRingZone(const core::vector3df& pos, const core::vector3df& normal, float minRadius, float maxRadius);

			void deleteZone(CZone *z);
		};
	}
}