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

#include "Utils/CSingleton.h"
#include "Graphics2D/SpriteFrame/IFont.h"
#include "Graphics2D/SpriteFrame/CSpriteFrame.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	class SKYLICHT_API CRenderTextData
	{
	public:
		core::vector3df Position;
		SColor Color;
		int ShaderID;
		CMaterial* Material;
		bool Visible;

	protected:
		std::wstring m_text;

		IFont* m_font;
		std::vector<std::vector<SModuleOffset*>> m_arrayCharRender;

		float m_textHeight;
		float m_textOffsetY;
		float m_linePadding;
		float m_spacePadding;
	public:

		CRenderTextData();

		~CRenderTextData();

		void init();

		void setText(IFont* font, const char* text);

		void setText(IFont* font, const wchar_t* text);

		void setText(const char* text);

		void setText(const wchar_t* text);

		inline std::vector<std::vector<SModuleOffset*>>& getModules()
		{
			return m_arrayCharRender;
		}

		inline float getTextHeight()
		{
			return m_textHeight;
		}

		inline float getLinePadding()
		{
			return m_linePadding;
		}

		inline void setLinePadding(float p)
		{
			m_linePadding = p;
		}

		inline float getSpacePadding()
		{
			return m_spacePadding;
		}

		inline void setSpacePadding(float p)
		{
			m_spacePadding = p;
		}

		inline float getTextOffsetY()
		{
			return m_textOffsetY;
		}
	};

	class SKYLICHT_API CTextBillboardManager
	{
	public:
		DECLARE_SINGLETON(CTextBillboardManager)

	protected:
		core::array<CRenderTextData*> m_texts;

		IFont* m_defaultFont;

		int m_defaultShader;

	public:

		CTextBillboardManager();

		virtual ~CTextBillboardManager();

		void clear();

		CRenderTextData* addText(const core::vector3df& position, const char* text);

		CRenderTextData* addText(const core::vector3df& position, const wchar_t* text);

		void initDefaultFont();

		const core::array<CRenderTextData*>& getRenderTexts()
		{
			return m_texts;
		}
	};

}