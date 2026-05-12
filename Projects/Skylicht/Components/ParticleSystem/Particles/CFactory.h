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
#include "Renderers/CCPURenderer.h"
#include "Renderers/CMeshParticleRenderer.h"

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
		/**
		 * @class CFactory
		 * @ingroup ParticleSystem
		 * @brief Factory class for creating emitters, zones, and renderers.
		 * @details Accessed via CParticleComponent::getParticleFactory().
		 */
		class COMPONENT_API CFactory
		{
		protected:
			std::vector<IRenderer*> m_renderers;
			std::vector<CZone*> m_zones;
			std::vector<CEmitter*> m_emitters;

		public:
			CFactory();

			virtual ~CFactory();

			/** @brief Creates a new Random emitter. */
			CRandomEmitter* createRandomEmitter();

			/** @brief Creates a new Straight emitter with a fixed direction. */
			CStraightEmitter* createStraightEmitter(const core::vector3df& direction);

			/** @brief Creates a new Spheric (cone) emitter. */
			CSphericEmitter* createSphericEmitter(const core::vector3df& direction, float angleA, float angleB);

			/** @brief Creates a new Normal emitter shooting along zone surfaces. */
			CNormalEmitter* createNormalEmitter(bool inverted);

			/** @brief Deletes and removes an emitter. */
			void deleteEmitter(CEmitter* e);

			/** @brief Creates a GPU instanced quad renderer. */
			CQuadRenderer* createQuadRenderer();

			/** @brief Creates a CPU-based quad renderer. */
			CCPURenderer* createCPURenderer();

			/** @brief Creates a GPU mesh instancing renderer. */
			CMeshParticleRenderer* createMeshParticleRenderer();

			/** @brief Deletes and removes a renderer. */
			void deleteRenderer(IRenderer* r);

			/** @brief Creates a zone based on type enum. */
			CZone* createZone(EZone type);

			/** @brief Creates a Point zone at origin. */
			CPoint* createPointZone();

			/** @brief Creates a Point zone at specific position. */
			CPoint* createPointZone(const core::vector3df& pos);

			/** @brief Creates a Sphere zone. */
			CSphere* createSphereZone(const core::vector3df& pos, float radius);

			/** @brief Creates an Axis-Aligned Box zone. */
			CAABox* createAABoxZone(const core::vector3df& pos, const core::vector3df& dimension);

			/** @brief Creates a Cylinder zone. */
			CCylinder* createCylinderZone(const core::vector3df& pos, const core::vector3df& direction, float radius, float length);

			/** @brief Creates a Line segment zone. */
			CLine* createLineZone(const core::vector3df& p1, const core::vector3df& p2);

			/** @brief Creates a multi-segment PolyLine zone. */
			CPolyLine* createPolyLineZone(const core::array<core::vector3df>& points);

			/** @brief Creates a 2D Ring zone in 3D space. */
			CRing* createRingZone(const core::vector3df& pos, const core::vector3df& normal, float minRadius, float maxRadius);

			/** @brief Deletes and removes a zone. */
			void deleteZone(CZone* z);

			/** @brief Creates an emitter by its class name (for XML loading). */
			CEmitter* createEmitter(const std::wstring& attributeName);

			/** @brief Creates a zone by its class name (for XML loading). */
			CZone* createZone(const std::wstring& attributeName);

			/** @brief Creates a renderer by its class name (for XML loading). */
			IRenderer* createRenderer(const std::wstring& attributeName);
		};
	}
}