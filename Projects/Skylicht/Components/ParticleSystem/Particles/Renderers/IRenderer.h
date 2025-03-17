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
		enum ERenderer
		{
			Quad,
			BillboardAddtive,
			MeshInstancing
		};

		class COMPONENT_API IRenderer : public CParticleSerializable
		{
		protected:
			ERenderer m_type;

			CMaterial* m_material;
			CMaterial* m_customMaterial;

			bool m_useCustomMaterial;
			bool m_useInstancing;
			bool m_emission;
			float m_emissionIntensity;

			std::string m_texturePath;
			ITexture* m_texture;

			u32 m_atlasNx;
			u32 m_atlasNy;

			std::string m_materialPath;

			bool m_needUpdateMesh;

		public:
			float SizeX;
			float SizeY;
			float SizeZ;

		public:
			IRenderer(ERenderer type);

			virtual ~IRenderer();

			inline ERenderer getType()
			{
				return m_type;
			}

			CMaterial* getMaterial();

			void setTexturePath(const char* path);

			void setTexture(int slot, ITexture* texture);

			inline void setEmission(bool b)
			{
				m_emission = b;
			}

			inline bool isEmission()
			{
				return m_emission;
			}

			inline bool useInstancing()
			{
				return m_useInstancing;
			}

			inline void setEmissionIntensity(float b)
			{
				m_emissionIntensity = b;
			}

			inline float getEmissionIntensity()
			{
				return m_emissionIntensity;
			}

			inline bool needUpdateMesh()
			{
				return m_needUpdateMesh;
			}

			inline const wchar_t* getName()
			{
				if (m_type == Quad)
					return L"Quad (Instancing)";
				else if (m_type == MeshInstancing)
					return L"Mesh (Instancing)";
				else
					return L"CPU Billboard";
			}

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			virtual void getParticleBuffer(IMeshBuffer* buffer) = 0;

			void setAtlas(u32 x, u32 y);

			inline u32 getAtlasX()
			{
				return m_atlasNx;
			}

			inline u32 getAtlasY()
			{
				return m_atlasNy;
			}

			inline u32 getTotalFrames()
			{
				return m_atlasNx * m_atlasNy;
			}

			inline float getFrameWidth()
			{
				return 1.0f / m_atlasNx;
			}

			inline float getFrameHeight()
			{
				return 1.0f / m_atlasNy;
			}
		};
	}
}