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

#include "ComponentsConfig.h"
#include "Material/CMaterial.h"
#include "ParticleSystem/Particles/CParticleSerializable.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @enum ERenderer
		 * @ingroup ParticleSystem
		 * @brief Available particle renderer types.
		 */
		enum ERenderer
		{
			Quad,
			CPURenderer,
			MeshInstancing
		};

		/**
		 * @class IRenderer
		 * @ingroup ParticleSystem
		 * @brief Base interface for particle renderers.
		 * @details Implementations define the visual shape of particles and how they are submitted to the GPU.
		 */
		class COMPONENT_API IRenderer : public CParticleSerializable
		{
		protected:
			/** @brief Type of the renderer. */
			ERenderer m_type;

			/** @brief Default material for the particles. */
			CMaterial* m_material;
			/** @brief Optional custom material. */
			CMaterial* m_customMaterial;

			/** @brief Whether custom material is enabled. */
			bool m_useCustomMaterial;
			/** @brief Whether this renderer uses hardware instancing. */
			bool m_useInstancing;
			/** @brief Whether the particles have emission. */
			bool m_emission;
			/** @brief Depth test flag. */
			bool m_ztest;
			/** @brief Emission intensity multiplier. */
			float m_emissionIntensity;

			/** @brief Primary texture path. */
			std::string m_texturePath;

			/** @brief Atlas horizontal frame count. */
			u32 m_atlasNx;
			/** @brief Atlas vertical frame count. */
			u32 m_atlasNy;

			/** @brief Path to custom material file. */
			std::string m_materialPath;

			/** @brief Internal: flag to trigger mesh buffer updates. */
			bool m_needUpdateMesh;

		public:
			/** @brief Default base scale X. */
			float SizeX;
			/** @brief Default base scale Y. */
			float SizeY;
			/** @brief Default base scale Z. */
			float SizeZ;

		public:
			IRenderer(ERenderer type);

			virtual ~IRenderer();

			/** @brief Gets renderer type enum. */
			inline ERenderer getType()
			{
				return m_type;
			}

			/** @brief Gets active material (custom or default). */
			CMaterial* getMaterial();

			/** @brief Gets default internal material. */
			inline CMaterial* getDefaultMaterial()
			{
				return m_material;
			}

			/** @brief Gets custom material if set. */
			inline CMaterial* getCustomMaterial()
			{
				return m_customMaterial;
			}

			/** @brief Loads and sets the main texture. */
			void setTexturePath(const char* path);

			/** @brief Sets texture for a specific slot. */
			void setTexture(int slot, ITexture* texture);

			/** @brief Enables/disables emission. */
			inline void setEmission(bool b)
			{
				m_emission = b;
			}

			/** @brief Checks emission status. */
			inline bool isEmission()
			{
				return m_emission;
			}

			/** @brief Checks instancing support. */
			inline bool useInstancing()
			{
				return m_useInstancing;
			}

			/** @brief Sets emission intensity. */
			inline void setEmissionIntensity(float b)
			{
				m_emissionIntensity = b;
			}

			/** @brief Gets emission intensity. */
			inline float getEmissionIntensity()
			{
				return m_emissionIntensity;
			}

			/** @brief Checks if internal mesh requires rebuilding. */
			inline bool needUpdateMesh()
			{
				return m_needUpdateMesh;
			}

			/** @brief Gets display name. */
			inline const wchar_t* getName()
			{
				if (m_type == Quad)
					return L"Quad (Instancing)";
				else if (m_type == MeshInstancing)
					return L"Mesh (Instancing)";
				else
					return L"CPU Renderer";
			}

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Implementation: fills the mesh buffer with renderer-specific geometry. */
			virtual void getParticleBuffer(IMeshBuffer* buffer) = 0;

			/** @brief Configures texture atlas dimensions. */
			void setAtlas(u32 x, u32 y);

			/** @brief Gets atlas column count. */
			inline u32 getAtlasX()
			{
				return m_atlasNx;
			}

			/** @brief Gets atlas row count. */
			inline u32 getAtlasY()
			{
				return m_atlasNy;
			}

			/** @brief Gets total frames in atlas. */
			inline u32 getTotalFrames()
			{
				return m_atlasNx * m_atlasNy;
			}

			/** @brief Gets UV width of a single frame. */
			inline float getFrameWidth()
			{
				return 1.0f / m_atlasNx;
			}

			/** @brief Gets UV height of a single frame. */
			inline float getFrameHeight()
			{
				return 1.0f / m_atlasNy;
			}
		};
	}
}