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

#include "Components/CComponentSystem.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		class CBakeLightComponent : public CComponentSystem
		{
		public:
			enum EBakeSize
			{
				Size512,
				Size1024,
				Size2048,
				Size4096
			};

		protected:
			bool m_bakeAll;
			bool m_bakeUV0;
			bool m_bakeDetailNormal;
			bool m_combineDirectionLightColor;

			std::string m_outputFolder;
			std::string m_outputFile;

			EBakeSize m_bakeSize;

		public:
			CBakeLightComponent();

			virtual ~CBakeLightComponent();

			virtual void initComponent();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			DECLARE_GETTYPENAME(CBakeLightComponent)

			inline bool bakeAll()
			{
				return m_bakeAll;
			}

			inline bool bakeUV0()
			{
				return m_bakeUV0;
			}

			inline bool bakeDetailNormal()
			{
				return m_bakeDetailNormal;
			}

			inline bool combineDirectionLightColor()
			{
				return m_combineDirectionLightColor;
			}

			u32 getBakeSize();

			inline const char* getOutputFolder()
			{
				return m_outputFolder.c_str();
			}

			inline const char* getOutputFile()
			{
				return m_outputFile.c_str();
			}
		};
	}
}