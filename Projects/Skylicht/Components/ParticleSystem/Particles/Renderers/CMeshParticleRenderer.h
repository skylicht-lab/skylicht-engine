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
		class COMPONENT_API CMeshParticleRenderer : public IRenderer
		{
		public:
			enum EBaseShaderType
			{
				Soild,
				SoildColor,
				Additive,
				Transparent
			};

		protected:
			std::string m_meshFile;

			IMeshBuffer* m_meshBuffer;

			EBaseShaderType m_baseShaderType;

			core::vector3df m_noiseScale;
			SColor m_dissolveColor;
			float m_dissolve;

			bool m_velocityDirection;

		public:
			CMeshParticleRenderer();

			virtual ~CMeshParticleRenderer();

			virtual void getParticleBuffer(IMeshBuffer* buffer);

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			DECLARE_GETTYPENAME(CMeshParticleRenderer)

			bool loadMesh(const char* meshFile);

			void setMaterialType(EBaseShaderType shader);

		protected:

			void initFromPrefab(CEntityPrefab* prefab);

			void addMesh(const core::matrix4& transform, CMesh* mesh);

		};
	}
}