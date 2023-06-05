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
#include "CLightmapData.h"

namespace Skylicht
{
	class CEntity;

	class CLightmap : public CComponentSystem
	{
	public:

	protected:
		std::vector<std::string> m_lightmapPaths;

		ITexture* m_lightmap;

		bool m_internalLightmap;

		std::vector<CLightmapData*> m_data;

	public:
		CLightmap();

		virtual ~CLightmap();

		virtual void initComponent();

		virtual void startComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

	protected:

		void addLightingData(CEntity* entity);

	public:

		void updateLightmap(bool loadLightmap = true);

		void setIndirectLightmap(ITexture* texture);

		ITexture* getIndirectLightmap()
		{
			return m_lightmap;
		}

		std::vector<CLightmapData*>& getData()
		{
			return m_data;
		}

		bool isLightmapEmpty();
		bool isLightmapChanged(const std::vector<std::string>& paths);

		DECLARE_GETTYPENAME(CLightmap)
	};
}