/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#include "CGUIElement.h"
#include "Graphics2D/SpriteFrame/IFont.h"

#define MAX_FORMATCOLOR 32

#define HAVE_CARET

namespace Skylicht
{
	class CFontSource;

	/**
	 * @brief This is the object class for displaying a text.
	 * @ingroup GUI
	 *
	 * @see CFontSource, CGlyphFont, CSpriteFont
	 *
	 * @code
	 * CFontSource* fontSource = CFontManager::getInstance()->loadFontSource("SampleGUI/Fonts/Roboto.font");
	 * IFont* font = NULL;
	 * if (fontSource)
	 * {
	 * 	fontSource->initFont();
	 * 	font = fontSource->getFont();
	 * }
	 *
	 * CCanvas *canvas = gameobject->addComponent<CCanvas>();
	 * core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	 * CGUIText* gui = canvas->createText(r, font);
	 * gui->setText("Hello");
	 * @endcode
	 */
	class SKYLICHT_API CGUIText : public CGUIElement
	{
		friend class CCanvas;
	public:
		typedef std::vector<int> ArrayInt;
		typedef std::vector<SModuleOffset*> ArrayModuleOffset;

	protected:
		IFont* m_font;
		IFont* m_customfont;

		int m_charPadding;
		int m_charSpacePadding;
		int m_linePadding;

		bool m_strim;

		std::string m_defaultText;

		std::string m_text;
		std::wstring m_textw;
		ArrayInt m_textFormat;

		bool m_enableTextFormat;
		bool m_password;

		std::string m_textId;
		std::wstring m_textwId;

		EGUIVerticalAlign TextVertical;
		EGUIHorizontalAlign TextHorizontal;

		int m_textHeight;
		int m_textOffsetY;

		bool m_multiLine;
		bool m_centerRotate;

		SColor m_colorFormat[MAX_FORMATCOLOR];

		bool m_updateTextRender;

		std::vector<ArrayModuleOffset> m_arrayCharRender;
		std::vector<ArrayInt> m_arrayCharFormat;
		std::vector<ArrayInt> m_arrayCharId;

		std::string m_fontSource;
		std::string m_fontGUID;

		CFontSource* m_fontData;
		int m_fontChanged;

		float m_lastWidth;
		float m_lastHeight;

#ifdef HAVE_CARET
		bool m_showCaret;
		int m_caretShader;
		int m_setCaret;
		float m_caretBlink;
		float m_caretBlinkSpeed;
		core::vector2di m_caret;
#endif

	protected:
		/**
		 * @brief Construct a text element with an initial font.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param font Font used for rendering text.
		 */
		CGUIText(CCanvas* canvas, CGUIElement* parent, IFont* font);

		/**
		 * @brief Construct a text element with an initial rectangle and font.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param rect Initial local GUI rectangle.
		 * @param font Font used for rendering text.
		 */
		CGUIText(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, IFont* font);

		/**
		 * @brief Render one prepared line of text modules.
		 * @param string Character module offsets to render.
		 * @param format Format color indices for the characters.
		 * @param posX X position of the line.
		 * @param posY Y position of the line.
		 * @param line Line index.
		 */
		virtual void renderText(ArrayModuleOffset& string, ArrayInt& format, int posX, int posY, int line);

		/**
		 * @brief Initialize default text rendering state.
		 */
		void init();

	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~CGUIText();

		/**
		 * @brief Render the text element.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void render(CCamera* camera);

		/**
		 * @brief Set horizontal and vertical text alignment inside the element rectangle.
		 * @param h Horizontal alignment.
		 * @param v Vertical alignment.
		 */
		void setTextAlign(EGUIHorizontalAlign h, EGUIVerticalAlign v)
		{
			TextVertical = v;
			TextHorizontal = h;
		}

		/*
		* Example:
		* <1>PLAYERA <0>gives <2>a heart <0>to <1>PLAYERB
		* With format:
		* <1> -> RED
		* <0> -> WHITE
		* <2> -> YELLOW
		*/
		/**
		 * @brief Set text that contains inline color format tags.
		 * @param formatText UTF-8 text with tags such as <0>, <1>, or <2>.
		 */
		void setFormatText(const char* formatText);

		/**
		 * @brief Set a color used by formatted text tags.
		 * @param id Format color index.
		 * @param c Color assigned to the index.
		 */
		void setColorFormat(int id, const SColor& c)
		{
			if (id < MAX_FORMATCOLOR)
				m_colorFormat[id] = c;
		}

		/**
		 * @brief Set the displayed UTF-8 text.
		 * @param text Text string.
		 */
		void setText(const char* text);

		/**
		 * @brief Set displayed UTF-8 text and enable string trimming.
		 * @param text Text string.
		 */
		void setTextStrim(const char* text);

		/**
		 * @brief Set the displayed wide-character text.
		 * @param text Wide text string.
		 */
		void setText(const wchar_t* text);

		/**
		 * @brief Set displayed wide-character text and enable string trimming.
		 * @param text Wide text string.
		 */
		void setTextStrim(const wchar_t* text);

		/**
		 * @brief Set the localization or resource ID for this text.
		 * @param textId UTF-8 text ID.
		 */
		void setTextId(const char* textId);

		/**
		 * @brief Set the localization or resource ID for this text.
		 * @param textId Wide text ID.
		 */
		void setTextId(const wchar_t* textId);

		/**
		 * @brief Measure the rendered width of a UTF-8 string.
		 * @param text Text string to measure.
		 * @return Width in pixels.
		 */
		int getStringWidth(const char* text);

		/**
		 * @brief Measure the rendered width of a wide string.
		 * @param text Text string to measure.
		 * @return Width in pixels.
		 */
		int getStringWidth(const wchar_t* text);

		/**
		 * @brief Measure the rendered width of a single character.
		 * @param c Character to measure.
		 * @return Width in pixels.
		 */
		int getCharWidth(wchar_t c);

		/**
		 * @brief Get the current UTF-8 text.
		 * @return Current text string.
		 */
		const char* getText()
		{
			return m_text.c_str();
		}

		/**
		 * @brief Get the default UTF-8 text.
		 * @return Default text string.
		 */
		const char* getDefaultText()
		{
			return m_defaultText.c_str();
		}

		/**
		 * @brief Set the default UTF-8 text used by this element.
		 * @param text Default text string.
		 */
		void setDefaultText(const char* text);

		/**
		 * @brief Set the default wide-character text used by this element.
		 * @param text Default wide text string.
		 */
		void setDefaultText(const wchar_t* text);

		/**
		 * @brief Get the current wide-character text.
		 * @return Current wide text string.
		 */
		const wchar_t* getTextW()
		{
			return m_textw.c_str();
		}

		/**
		 * @brief Get the UTF-8 text resource ID.
		 * @return Text ID string.
		 */
		const char* getTextId()
		{
			return m_textId.c_str();
		}

		/**
		 * @brief Get the wide-character text resource ID.
		 * @return Wide text ID string.
		 */
		const wchar_t* getTextwId()
		{
			return m_textwId.c_str();
		}

		/**
		 * @brief Get the number of wide characters in the current text.
		 * @return Text length in characters.
		 */
		inline int getTextLength()
		{
			return (int)m_textw.size();
		}

		/**
		 * @brief Check whether trimming is enabled for the current text.
		 * @return True if text trimming is enabled.
		 */
		inline bool isStrim()
		{
			return m_strim;
		}

		/**
		 * @brief Enable or disable inline text formatting.
		 * @param b True to enable formatted text parsing.
		 */
		inline void setEnableTextFormnat(bool b)
		{
			m_enableTextFormat = b;
		}

		/**
		 * @brief Check whether inline text formatting is enabled.
		 * @return True if text formatting is enabled.
		 */
		inline bool isEnableTextFormat()
		{
			return m_enableTextFormat;
		}

		/**
		 * @brief Enable or disable password masking for displayed text.
		 * @param b True to display password mask characters.
		 */
		void setPassword(bool b);

		/**
		 * @brief Check whether password masking is enabled.
		 * @return True if password masking is enabled.
		 */
		inline bool isPassword()
		{
			return m_password;
		}

		/**
		 * @brief Set horizontal padding applied between characters.
		 * @param charPadding Padding in pixels.
		 */
		inline void setCharPadding(int charPadding)
		{
			m_charPadding = charPadding;
			m_charSpacePadding = charPadding;
		}

		/**
		 * @brief Set vertical padding applied between text lines.
		 * @param linePadding Padding in pixels.
		 */
		inline void setLinePadding(int linePadding)
		{
			m_linePadding = linePadding;
		}

		/**
		 * @brief Enable or disable multiline text layout.
		 * @param b True to allow multiple lines.
		 */
		inline void setMultiLine(bool b)
		{
			m_multiLine = b;
		}

		/**
		 * @brief Enable or disable center-based rotation for text rendering.
		 * @param b True to rotate around the text center.
		 */
		inline void setCenterRotate(bool b)
		{
			m_centerRotate = b;
		}

		/**
		 * @brief Load and assign a font from a font source path.
		 * @param fontSource Font source resource path.
		 */
		void setFontSource(const char* fontSource);

		/**
		 * @brief Assign a font source object directly.
		 * @param fontData Font source object.
		 */
		void setFontSource(CFontSource* fontData);

		/**
		 * @brief Assign a custom font used instead of the font source.
		 * @param font Custom font instance.
		 */
		void setCustomFont(IFont* font);

		/**
		 * @brief Get the custom font assigned to this text element.
		 * @return Custom font instance, or NULL if none is assigned.
		 */
		inline IFont* getCustomFont()
		{
			return m_customfont;
		}

		/**
		 * @brief Get the font source object used by this text element.
		 * @return Font source object.
		 */
		inline CFontSource* getFontData()
		{
			return m_fontData;
		}

		/**
		 * @brief Get the font source path.
		 * @return Font source path string.
		 */
		const std::string& getFontSource()
		{
			return m_fontSource;
		}

		/**
		 * @brief Get the serialized font ID.
		 * @return Font ID string.
		 */
		const std::string& getFontId()
		{
			return m_fontGUID;
		}

		DECLARE_GETTYPENAME(CGUIText)

		/**
		 * @brief Create a serializable object that stores this text state.
		 * @return Serializable object.
		 */
		virtual CObjectSerializable* createSerializable();

		/**
		 * @brief Load text state from a serializable object.
		 * @param object Serializable object to read from.
		 */
		virtual void loadSerializable(CObjectSerializable* object);

		/**
		 * @brief Find the closest character index to a GUI-space position.
		 * @param posX X position.
		 * @param posY Y position.
		 * @param line Output line index.
		 * @param character Output character index.
		 */
		void getClosestCharacter(float posX, float posY, int& line, int& character);

#ifdef HAVE_CARET
		/**
		 * @brief Show or hide the text caret.
		 * @param b True to show the caret.
		 */
		inline void showCaret(bool b)
		{
			m_showCaret = b;
		}

		/**
		 * @brief Set caret position by line and character index.
		 * @param line Line index.
		 * @param character Character index in the line.
		 */
		inline void setCaret(int line, int character)
		{
			m_caret.set(character, line);
			m_caretBlink = 0.0f;
		}

		/**
		 * @brief Set caret position by absolute character index.
		 * @param charPos Absolute character index.
		 */
		void setCaret(int charPos);

		/**
		 * @brief Get the current caret position.
		 * @param line Output line index.
		 * @param character Output character index in the line.
		 */
		inline void getCaretPosition(int& line, int& character)
		{
			character = m_caret.X;
			line = m_caret.Y;
		}

		/**
		 * @brief Get the number of rendered text lines.
		 * @return Number of lines.
		 */
		inline int getNumLine()
		{
			return (int)m_arrayCharRender.size();
		}

		/**
		 * @brief Get the number of characters in a rendered line.
		 * @param line Line index.
		 * @return Number of characters in the line.
		 */
		int getNumCharacter(int line);

		/**
		 * @brief Get the word range at a character position.
		 * @param line Line index.
		 * @param charPosition Character index in the line.
		 * @param from Output start character index.
		 * @param to Output end character index.
		 * @return True if a word range was found.
		 */
		bool getWordAtPosition(int line, int charPosition, int& from, int& to);

		/**
		 * @brief Check whether a character is considered part of a word.
		 * @param c Character to test.
		 * @return True if the character is a word character.
		 */
		bool isCharacter(wchar_t c);

		/**
		 * @brief Delete the character before the caret.
		 */
		void doBackspace();

		/**
		 * @brief Delete the character at the caret.
		 */
		void doDelete();

		/**
		 * @brief Insert a character at the caret.
		 * @param c Character to insert.
		 */
		void insert(wchar_t c);
#endif

	protected:

#ifdef HAVE_CARET
		/**
		 * @brief Update caret blink and render state.
		 */
		void updateCaret();

		/**
		 * @brief Apply a pending caret position update.
		 */
		void updateSetCaret();
#endif

		/**
		 * @brief Rebuild cached split text data for the current text and rectangle.
		 */
		void updateSplitText();

		/**
		 * @brief Split text into renderable lines, format ranges, and character IDs.
		 * @param split Output line module offsets.
		 * @param format Output line format indices.
		 * @param id Output line character IDs.
		 * @param width Available line width.
		 */
		void splitText(std::vector<ArrayModuleOffset>& split, std::vector<ArrayInt>& format, std::vector<ArrayInt>& id, int width);

		/**
		 * @brief Find the closest character within a prepared line.
		 * @param posX X position to test.
		 * @param x Line origin X.
		 * @param y Line origin Y.
		 * @param line Line index.
		 * @param world World transform used for hit testing.
		 * @param character Output character index.
		 */
		void getClosestCharacter(float posX, int x, int y, int line, const core::matrix4& world, int& character);

		/**
		 * @brief Convert a source character to the glyph displayed by this element.
		 * @param c Source character.
		 * @return Display character, including password mask substitution when enabled.
		 */
		wchar_t getDisplayCharacter(wchar_t c) const;

		/**
		 * @brief Initialize font-dependent cached data.
		 * @param font Font instance to initialize.
		 */
		void initFont(IFont* font);

		/**
		 * @brief Get the font currently used for rendering.
		 * @return Custom font when present, otherwise the source font.
		 */
		IFont* getCurrentFont();
	};
}
