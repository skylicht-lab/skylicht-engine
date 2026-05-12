/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#include "IRenderer.h"
#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CMeshParticleRenderer
		 * @ingroup ParticleSystem
		 * @brief Renderer that uses hardware instancing to draw 3D meshes as particles.
		 * 
		 * ### Example
		 * @code
		 * Particle::CMeshParticleRenderer *renderer = factory->createMeshParticleRenderer();
		 * renderer->loadMesh("Models/Stone/stone.fbx");
		 * renderer->setMaterialType(Particle::CMeshParticleRenderer::Soild);
		 * group->setRenderer(renderer);
		 * @endcode
		 */
		class COMPONENT_API CMeshParticleRenderer : public IRenderer
		{
		public:
			/**
			 * @enum EBaseShaderType
			 * @brief Blending modes for mesh particles.
			 */
			enum EBaseShaderType
			{
				Soild,			/**< Opaque mesh. */
				SoildColor,		/**< Opaque with vertex color. */
				Additive,		/**< Additive blending. */
				Transparent		/**< Alpha blending. */
			};

		protected:
			/** @brief Path to the source mesh file. */
			std::string m_meshFile;

			/** @brief Aggregated mesh buffer for instancing. */
			IMeshBuffer* m_meshBuffer;

			/** @brief Current shader mode. */
			EBaseShaderType m_baseShaderType;

			/** @brief Scale for dissolve noise effect. */
			core::vector3df m_noiseScale;
			/** @brief Edge color for dissolve effect. */
			SColor m_dissolveColor;
			/** @brief Cutoff value for dissolve effect. */
			float m_dissolve;

			/** @brief Whether to align mesh Z-axis to particle velocity. */
			bool m_velocityDirection;

		public:
			CMeshParticleRenderer();

			virtual ~CMeshParticleRenderer();

			/** @brief Internal: merges prefab meshes into a single buffer. */
			virtual void getParticleBuffer(IMeshBuffer* buffer);

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			DECLARE_GETTYPENAME(CMeshParticleRenderer)

			/** @brief Loads a 3D model (FBX, DAE, OBJ) to be used as a particle. */
			bool loadMesh(const char* meshFile);

			/** @brief Enables/disables velocity-based orientation. */
			void setVelocityDirection(bool b);

			/** @brief Configures blending mode and shader. */
			void setMaterialType(EBaseShaderType shader);

			/** @brief Gets dissolve edge color. */
			inline const SColor& getDissolveColor()
			{
				return m_dissolveColor;
			}

			/** @brief Sets dissolve edge color. */
			inline void setDissolveColor(const SColor& c)
			{
				m_dissolveColor = c;
			}

			/** @brief Gets dissolve noise scale. */
			inline core::vector3df& getNoiseScale()
			{
				return m_noiseScale;
			}

			/** @brief Sets dissolve noise scale. */
			inline void setNoiseScale(const core::vector3df& n)
			{
				m_noiseScale = n;
			}

			/** @brief Gets current dissolve progression. */
			inline float getDissolveCutoff()
			{
				return m_dissolve;
			}

			/** @brief Sets dissolve progression (0.0 to 1.0). */
			inline void setDissolveCutoff(float d)
			{
				m_dissolve = d;
			}

		protected:

			/** @brief Internal: extracts static meshes from a prefab. */
			void initFromPrefab(CEntityPrefab* prefab);

			/** @brief Internal: adds a transformed mesh to the instancing buffer. */
			void addMesh(const core::matrix4& transform, CMesh* mesh);

		};
	}
}