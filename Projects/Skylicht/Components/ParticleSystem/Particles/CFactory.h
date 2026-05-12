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

			/**
			 * @brief Create a random emitter that shoots particles in all directions.
			 * @return Pointer to CRandomEmitter.
			 */
			CRandomEmitter* createRandomEmitter();

			/**
			 * @brief Create a straight emitter with a fixed direction.
			 * @param direction Emission direction vector.
			 * @return Pointer to CStraightEmitter.
			 */
			CStraightEmitter* createStraightEmitter(const core::vector3df& direction);

			/**
			 * @brief Create a spheric emitter that shoots particles within a cone.
			 * @param direction Primary axis of the cone.
			 * @param angleA Inner angle of the cone (in radians).
			 * @param angleB Outer angle of the cone (in radians).
			 * @return Pointer to CSphericEmitter.
			 */
			CSphericEmitter* createSphericEmitter(const core::vector3df& direction, float angleA, float angleB);

			/**
			 * @brief Create a normal emitter that shoots particles along zone surface normals.
			 * @param inverted If true, shoots particles into the zone.
			 * @return Pointer to CNormalEmitter.
			 */
			CNormalEmitter* createNormalEmitter(bool inverted);

			/**
			 * @brief Delete an emitter and remove it from internal tracking.
			 * @param e Pointer to the emitter to delete.
			 */
			void deleteEmitter(CEmitter* e);

			/**
			 * @brief Create an optimized GPU quad renderer for billboard particles.
			 * @return Pointer to CQuadRenderer.
			 */
			CQuadRenderer* createQuadRenderer();

			/**
			 * @brief Create a CPU-based renderer (compatibility mode).
			 * @return Pointer to CCPURenderer.
			 */
			CCPURenderer* createCPURenderer();

			/**
			 * @brief Create a hardware instancing renderer for 3D meshes.
			 * @return Pointer to CMeshParticleRenderer.
			 */
			CMeshParticleRenderer* createMeshParticleRenderer();

			/**
			 * @brief Delete a renderer.
			 * @param r Pointer to the renderer to delete.
			 */
			void deleteRenderer(IRenderer* r);

			/**
			 * @brief Create a spawn zone by type.
			 * @param type EZone enum value.
			 * @return Pointer to the new CZone.
			 */
			CZone* createZone(EZone type);

			/**
			 * @brief Create a point spawn zone at (0,0,0).
			 * @return Pointer to CPoint.
			 */
			CPoint* createPointZone();

			/**
			 * @brief Create a point spawn zone at a specific position.
			 * @param pos Position vector.
			 * @return Pointer to CPoint.
			 */
			CPoint* createPointZone(const core::vector3df& pos);

			/**
			 * @brief Create a spherical spawn zone.
			 * @param pos Center position.
			 * @param radius Sphere radius.
			 * @return Pointer to CSphere.
			 */
			CSphere* createSphereZone(const core::vector3df& pos, float radius);

			/**
			 * @brief Create an axis-aligned box spawn zone.
			 * @param pos Center position.
			 * @param dimension Box dimensions (width, height, depth).
			 * @return Pointer to CAABox.
			 */
			CAABox* createAABoxZone(const core::vector3df& pos, const core::vector3df& dimension);

			/**
			 * @brief Create a cylindrical spawn zone.
			 * @param pos Center position.
			 * @param direction Cylinder axis direction.
			 * @param radius Cylinder radius.
			 * @param length Cylinder length.
			 * @return Pointer to CCylinder.
			 */
			CCylinder* createCylinderZone(const core::vector3df& pos, const core::vector3df& direction, float radius, float length);

			/**
			 * @brief Create a line segment spawn zone.
			 * @param p1 Start point.
			 * @param p2 End point.
			 * @return Pointer to CLine.
			 */
			CLine* createLineZone(const core::vector3df& p1, const core::vector3df& p2);

			/**
			 * @brief Create a poly-line spawn zone from a sequence of points.
			 * @param points Array of position vectors.
			 * @return Pointer to CPolyLine.
			 */
			CPolyLine* createPolyLineZone(const core::array<core::vector3df>& points);

			/**
			 * @brief Create a ring spawn zone in 3D space.
			 * @param pos Center position.
			 * @param normal Plane normal vector.
			 * @param minRadius Inner radius.
			 * @param maxRadius Outer radius.
			 * @return Pointer to CRing.
			 */
			CRing* createRingZone(const core::vector3df& pos, const core::vector3df& normal, float minRadius, float maxRadius);

			/**
			 * @brief Delete a zone.
			 * @param z Pointer to the zone to delete.
			 */
			void deleteZone(CZone* z);

			/**
			 * @brief Create an emitter by type name (for serialization).
			 * @param attributeName Wstring identifier.
			 * @return Pointer to CEmitter.
			 */
			CEmitter* createEmitter(const std::wstring& attributeName);

			/**
			 * @brief Create a zone by type name (for serialization).
			 * @param attributeName Wstring identifier.
			 * @return Pointer to CZone.
			 */
			CZone* createZone(const std::wstring& attributeName);

			/**
			 * @brief Create a renderer by type name (for serialization).
			 * @param attributeName Wstring identifier.
			 * @return Pointer to IRenderer.
			 */
			IRenderer* createRenderer(const std::wstring& attributeName);
		};
	}
}