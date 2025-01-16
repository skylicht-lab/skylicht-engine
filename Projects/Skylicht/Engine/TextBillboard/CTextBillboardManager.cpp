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

#include "pch.h"
#include "CTextBillboardManager.h"

#include "Graphics2D/SpriteFrame/CSpriteFont.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CRenderTextData::CRenderTextData() :
		m_font(NULL),
		Color(255, 255, 255, 255),
		m_textHeight(0.0f),
		m_textOffsetY(0.0f),
		m_linePadding(0.0f),
		m_spacePadding(0.0f),
		ShaderID(0),
		Material(NULL),
		Visible(true),
		VAlign(CRenderTextData::Top),
		HAlign(CRenderTextData::Left),
		UserData(NULL)

	{

	}

	CRenderTextData::~CRenderTextData()
	{

	}

	void CRenderTextData::init()
	{
		std::vector<SModuleOffset*> modules;

		m_arrayCharRender.clear();
		const wchar_t* lpString = m_text.c_str();

		if (m_font == NULL)
			return;

		// get text height
		SModuleOffset* moduleCharA = m_font->getCharacterModule((int)'A');
		if (moduleCharA)
		{
			m_textHeight = moduleCharA->OffsetY + moduleCharA->Module->H;
			m_textOffsetY = moduleCharA->OffsetY;
		}

		int i = 0;
		while (lpString[i] != 0)
		{
			if (lpString[i] == '\n')
			{
				m_arrayCharRender.push_back(modules);
				modules.clear();

				i++;
				continue;
			}
			else if (lpString[i] == '\\')
			{
				if (lpString[i + 1] == 'n')
				{
					m_arrayCharRender.push_back(modules);
					modules.clear();

					i += 2;
					continue;
				}
			}

			SModuleOffset* c = m_font->getCharacterModule((int)lpString[i]);
			if (c != NULL)
			{
				modules.push_back(c);
			}
			i++;
		}

		m_arrayCharRender.push_back(modules);
	}

	void CRenderTextData::setText(IFont* font, const char* text)
	{
		m_font = font;
		m_text = CStringImp::convertUTF8ToUnicode(text).c_str();
		init();
	}

	void CRenderTextData::setText(IFont* font, const wchar_t* text)
	{
		m_font = font;
		m_text = text;
		init();
	}

	void CRenderTextData::setText(const char* text)
	{
		m_text = CStringImp::convertUTF8ToUnicode(text).c_str();
		init();
	}

	void CRenderTextData::setText(const wchar_t* text)
	{
		m_text = text;
		init();
	}

	IMPLEMENT_SINGLETON(CTextBillboardManager);

	CTextBillboardManager::CTextBillboardManager() :
		m_defaultFont(NULL),
		m_defaultShader(0),
		m_viewportWidth(0),
		m_viewportHeight(0)
	{

	}

	CTextBillboardManager::~CTextBillboardManager()
	{
		if (m_defaultFont)
			m_defaultFont->dropFont();

		clear();
	}

	void CTextBillboardManager::clear()
	{
		for (u32 i = 0, n = m_texts.size(); i < n; i++)
		{
			delete m_texts[i];
		}
		m_texts.set_used(0);
	}

	void CTextBillboardManager::remove(CRenderTextData* text)
	{
		for (u32 i = 0, n = m_texts.size(); i < n; i++)
		{
			if (m_texts[i] == text)
			{
				delete m_texts[i];
				m_texts.erase(i);
				return;
			}
		}
	}

	void CTextBillboardManager::initDefaultFont()
	{
		if (!m_defaultFont)
		{
			CSpriteFont* spriteFont = new CSpriteFont();
			spriteFont->loadFont("BuiltIn/Fonts/droidsans-18.fnt");

			m_defaultFont = spriteFont;
		}
	}

	CRenderTextData* CTextBillboardManager::addText(const core::vector3df& position, const char* text)
	{
		if (!m_defaultFont)
			initDefaultFont();

		if (!m_defaultFont)
			return NULL;

		return addText(position, text, m_defaultFont);
	}

	CRenderTextData* CTextBillboardManager::addText(const core::vector3df& position, const wchar_t* text)
	{
		if (!m_defaultFont)
			initDefaultFont();

		if (!m_defaultFont)
			return NULL;

		return addText(position, text, m_defaultFont);
	}

	CRenderTextData* CTextBillboardManager::addText(const core::vector3df& position, const char* text, IFont* font)
	{
		if (font == NULL)
			return NULL;

		if (m_defaultShader == 0)
			m_defaultShader = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");

		CRenderTextData* t = new CRenderTextData();
		m_texts.push_back(t);

		t->Position = position;
		t->ShaderID = m_defaultShader;
		t->setText(font, text);

		return t;
	}

	CRenderTextData* CTextBillboardManager::addText(const core::vector3df& position, const wchar_t* text, IFont* font)
	{
		if (font == NULL)
			return NULL;

		if (m_defaultShader == 0)
			m_defaultShader = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");

		CRenderTextData* t = new CRenderTextData();
		m_texts.push_back(t);

		t->Position = position;
		t->ShaderID = m_defaultShader;
		t->setText(font, text);

		return t;
	}
}