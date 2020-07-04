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

#include "Components/CComponentSystem.h"
#include "CIndirectLightingData.h"

namespace Skylicht
{
	class CEntity;

	class CIndirectLighting : public CComponentSystem
	{
	public:
		enum EIndirectType
		{
			Lightmap,
			LightmapArray,
			VertexColor,
			SH4,
			SH9,
		};

	protected:
		EIndirectType m_type;
		int m_lightmapIndex;

		std::vector<CIndirectLightingData*> m_data;

		core::vector3df m_sh[9];
		ITexture *m_lightmap;

	public:
		CIndirectLighting();

		virtual ~CIndirectLighting();

		virtual void initComponent();

		virtual void updateComponent();

	protected:

		void addLightingData(CEntity *entity);

	public:

		void setIndirectLightingType(EIndirectType type);

		inline void setLightmapIndex(int idx)
		{
			m_lightmapIndex = idx;
		}

		inline int getLightmapIndex()
		{
			return m_lightmapIndex;
		}

		void setLightmap(ITexture *texture);

		ITexture* getLightmap()
		{
			return m_lightmap;
		}

		void setSH(core::vector3df* sh);

		EIndirectType getIndirectLightingType()
		{
			return m_type;
		}

		std::vector<CIndirectLightingData*>& getData()
		{
			return m_data;
		}
	};
}