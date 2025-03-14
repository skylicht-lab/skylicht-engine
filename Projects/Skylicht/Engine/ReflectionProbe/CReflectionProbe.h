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

		DECLARE_GETTYPENAME(CReflectionProbe)
	};
}