/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#ifdef BUILD_SKYLICHT_LIGHMAPPER

#include "SkylichtEngine.h"

#include "Editor/Space/CSpaceLoading.h"
#include "AssetManager/CAssetImporter.h"
#include "AssetManager/CAssetManager.h"

#include "BakeLightComponent/CBakeLightComponent.h"

#include "BakeDirectionalRP/CShadowMapBakeRP.h"
#include "BakeDirectionalRP/CDirectionalLightBakeRP.h"
#include "BakePointLightRP/CPointLightShadowBakeRP.h"
#include "BakePointLightRP/CPointLightBakeRP.h"

#define MAX_LIGHTMAP_ATLAS 10

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceBakeDirectional : public CSpaceLoading
		{
		protected:
			enum EBakeState
			{
				None = 0,
				Init,
				Bake,
				Finish,
			};

		protected:
			GUI::CProgressBar* m_progressBar;
			GUI::CLabel* m_statusText;

			EBakeState m_state;

			int m_position;
			int m_total;

			std::string m_folder;
			std::string m_file;
			float m_shadowBias;
			bool m_bakeAll;
			bool m_bakeUV0;
			bool m_bakeDetailNormal;
			bool m_combineDirectionLightColor;
			u32 m_bakeSize;

			CGameObject* m_cameraObj;
			std::vector<CLight*> m_lights;

			std::vector<IMeshBuffer*> m_meshBuffers;
			std::vector<core::matrix4> m_meshTransforms;
			std::vector<ITexture*> m_normalMaps;

			CShadowMapBakeRP* m_shadowRP;
			CDirectionalLightBakeRP* m_bakeLightRP;

			CPointLightShadowBakeRP* m_shadowPLRP;
			CPointLightBakeRP* m_bakePointLightRP;

			ITexture* m_directionLightBake[MAX_LIGHTMAP_ATLAS];
			ITexture* m_pointLightBake[MAX_LIGHTMAP_ATLAS];
			ITexture* m_result[MAX_LIGHTMAP_ATLAS];
			IMeshBuffer* m_subMesh[MAX_LIGHTMAP_ATLAS];

		public:
			CSpaceBakeDirectional(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceBakeDirectional();

			virtual void update();

			virtual void onDestroy(GUI::CBase* base);

			void bake(Lightmapper::CBakeLightComponent* bakeLight);

		protected:

			void doBakeUV0();

			void doBakeLightmap();

			void saveOutput();
		};
	}
}

#endif