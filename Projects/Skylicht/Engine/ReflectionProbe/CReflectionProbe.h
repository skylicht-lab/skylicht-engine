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

#include "Camera/CCamera.h"
#include "RenderPipeline/IRenderPipeline.h"
#include "RenderPipeline/CBaseRP.h"

#include "CReflectionProbeData.h"

namespace Skylicht
{
	enum class EReflectionType
	{
		Static,
		Baked,
	};

	enum class EReflectionSize
	{
		X64,
		X128,
		X256,
		X512,
		X1024,
		X2048
	};

	/// @brief The object class enables mapping reflection onto the Object.
	/// @ingroup IndirectLighting
	/// 
	/// CReflectionProbe supports loading reflection images from a texture file, or it can be baked directly from the 3D scene.
	/// 
	/// You can use Skylicht-Editor to select a texture, bake, or save the baked results to a reflection cube texture.
	/// 
	/// @image html Lighting/reflection-probe.jpg "PBR-shaded materials will select the nearest Reflection Probe" width=1200px
	class SKYLICHT_API CReflectionProbe : public CComponentSystem
	{
	protected:
		video::ITexture* m_staticTexture;

		video::ITexture* m_bakedTexture;

		std::string m_staticPath;

		EReflectionSize m_size;

		core::dimension2du m_bakeSize;

		video::ITexture* m_bakeTexture[6];

		CReflectionProbeData* m_probeData;

		EReflectionType m_type;

		video::ITexture* m_textureWillRemoved;

		u32 m_lightLayers;

	protected:

		void removeBakeTexture();

		void removeStaticTexture();

	public:
		CReflectionProbe();

		virtual ~CReflectionProbe();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		bool loadStaticTexture(const char* path);

		void bakeProbe(CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr);

		void bakeProbeToFile(CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr, const char* outfolder, const char* outname);

		video::ITexture* getReflectionTexture();

		inline u32 getLightLayers()
		{
			return m_lightLayers;
		}

		void setLightLayers(u32 layers);

		DECLARE_GETTYPENAME(CReflectionProbe)
	};
}