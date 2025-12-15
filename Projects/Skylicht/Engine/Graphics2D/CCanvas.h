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

#include "Components/CComponentSystem.h"

#include "Camera/CCamera.h"

#include "GUI/CGUIElement.h"
#include "GUI/CGUIImage.h"
#include "GUI/CGUIText.h"
#include "GUI/CGUISprite.h"
#include "GUI/CGUIRect.h"
#include "GUI/CGUIMask.h"
#include "GUI/CGUILayout.h"
#include "GUI/CGUIFitSprite.h"
#include "GUI/CGUIElipse.h"

#include "Entity/CEntityPrefab.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	/// @brief This class manages GUI components, including creating and deleting images and sprites.
	/// @ingroup Graphics2D
	/// 
	/// Canvas for managing GUI components and their hierarchy, rendering, and interaction.
	/// CCanvas serves as the root for all GUI elements rendered on screen. It supports element creation,
	/// scaling, resizing, hit - testing, and access by ID or path.
	/// 
	/// GUI elements can be designed visually in Skylicht-Editor, or loaded from .gui files using CGUIImporter.
	/// 
	/// Example usage:
	/// @code
	/// // Create an image element
	/// CGUIImage* image = canvas->createImage(rect);
	/// @endcode
	/// 
	/// **Skylicht-Editor**
	/// 
	/// To design the GUI, open the Skylicht-Editor and switch to the GUI design window.
	/// 
	/// @image html Graphics2D/CCanvas/gui-design.jpg "The interface includes features for designing GUIs for Canvas." width=1200px
	/// 
	/// **Setting**
	/// 
	/// @image html Graphics2D/CCanvas/gui-design-tool.jpg "You can adjust the base size of the Canvas in the Settings section." width=1200px
	/// 
	/// **Working with .gui Files**
	/// 
	/// You can save the design file to .gui formats and use CGUIImporter to load it into the Canvas.
	/// 
	/// @image html Graphics2D/CCanvas/gui-design-sample.jpg "You can refer to the example file Assets\SampleGUIDemo\Setting.gui and the codes in Samples\GUI," width=1200px
	class SKYLICHT_API CCanvas : public CComponentSystem
	{
	protected:
		/// The rectangle representing the canvas size.
		core::rectf m_rect;

		/// The default rectangle size of the canvas.
		core::rectf m_defaultRect;

		/// The root GUI element of the canvas.
		CGUIElement* m_root;

		/// Sorting depth for rendering.
		int m_sortDepth;

		/// Scale factor for GUI.
		float m_scaleGUI;

		/// True if GUI scaling is applied.
		bool m_haveScaleGUI;

		/// Enable 3D billboard rendering for the canvas.
		bool m_is3DBillboard;

		/// The world transform used for rendering.
		core::matrix4 m_renderWorldTransform;

		/// The camera currently used for rendering.
		CCamera* m_renderCamera;

		/// The current mask applied during rendering.
		CGUIMask* m_currentMask;

		/// Entity manager for managing GUI entities.
		CEntityPrefab* m_entityMgr;

		/// List of entity systems for GUI logic.
		std::vector<IEntitySystem*> m_systems;

		/// Entities sorted by children depth.
		CFastArray<CEntity*> m_depth[MAX_ENTITY_DEPTH];

		/// List of alive entities.
		CFastArray<CEntity*> m_alives;

	public:
		/// True if the canvas is shown in editor mode.
		bool IsInEditor;

		/// True to draw outlines for GUI elements.
		bool DrawOutline;

	public:

		std::function<void(CGUIElement*)> OnGUILoaded;

		std::function<void(CGUIText*)> OnLocalize;

	public:
		/**
		 * @brief Constructor.
		 */
		CCanvas();

		/**
		 * @brief Destructor. Cleans up GUI elements and systems.
		 */
		virtual ~CCanvas();

		/**
		 * @brief Initialize the canvas component.
		 */
		virtual void initComponent();

		/**
		 * @brief Update the canvas component.
		 */
		virtual void updateComponent();

		/**
		* @brief Handle canvas resizing (usually called when screen size changes).
		*/
		virtual void onResize();

		/**
		 * @brief Update internal entity lists and sorting for rendering.
		 */
		void updateEntities();

		/**
		 * @brief Render the GUI elements tree using the specified camera.
		 * @param camera The camera to use for rendering.
		 */
		void render(CCamera* camera);

		/**
		 * @brief Perform hit-testing to find the GUI element under the given coordinates.
		 * @param camera The camera used for projection.
		 * @param x Screen X coordinate.
		 * @param y Screen Y coordinate.
		 * @param viewport The viewport rectangle.
		 * @return Pointer to the hit CGUIElement, or NULL if none.
		 */
		CGUIElement* getHitTest(CCamera* camera, float x, float y, const core::recti& viewport);

		/**
		 * @brief Get the entity manager.
		 * @return Pointer to CEntityPrefab.
		 */
		inline CEntityPrefab* getEntityManager()
		{
			return m_entityMgr;
		}

	public:
		/**
		* @brief Set the sort depth for rendering.
		* @param d Depth value.
		*/
		inline void setSortDepth(int d)
		{
			m_sortDepth = d;
		}

		/**
		 * @brief Get the current sort depth value.
		 * @return Depth value.
		 */
		inline int getSortDepth()
		{
			return m_sortDepth;
		}

		/**
		 * @brief Set the canvas rectangle.
		 * @param r Rectangle value.
		 */
		inline void setRect(const core::rectf& r)
		{
			m_rect = r;
		}

		/**
		* @brief Set the default rectangle value.
		* @param r Rectangle value.
		*/
		inline void setDefaultRect(const core::rectf& r)
		{
			m_defaultRect = r;
		}

		/**
		 * @brief Get the default rectangle.
		 * @return Default rectangle.
		 */
		inline const core::rectf& getDefaultRect()
		{
			return m_defaultRect;
		}

		/**
		 * @brief Apply GUI scaling based on width or height ratio.
		 *
		 * If widthOrHeight = 0.0, the scaling will prioritize the width of the canvas.
		 * If widthOrHeight = 1.0, the scaling will prioritize the height of the canvas.
		 * Values between 0.0 and 1.0 interpolate between width-priority (0.0) and height-priority (1.0).
		 *
		 * @param widthOrHeight Scale value (default = 1.0).
		 */
		void applyGUIScale(float widthOrHeight = 1.0f);

		/**
		 * @brief Reset the GUI scaling to default (no scale).
		 */
		void resetGUIScale();

		/**
		 * @brief Get the root element's scale vector.
		 * @return Reference to the scale vector.
		 */
		const core::vector3df& getRootScale();

		/**
		 * @brief Get the root GUI element.
		 * @return Pointer to the root CGUIElement.
		 */
		inline CGUIElement* getRootElement()
		{
			return m_root;
		}

		/**
		 * @brief Enable or disable 3D billboard rendering.
		 * @param b True to enable.
		 */
		inline void setEnable3DBillboard(bool b)
		{
			m_is3DBillboard = b;
		}

		/**
		 * @brief Check if 3D billboard rendering is enabled.
		 * @return True if enabled.
		 */
		inline bool is3DBillboardEnabled()
		{
			return m_is3DBillboard;
		}

		/**
		 * @brief Remove all GUI elements from the canvas.
		 */
		void removeAllElements();

		/**
		 * @brief Create a generic GUI element as a child of root.
		 * @return Pointer to the created CGUIElement.
		 */
		CGUIElement* createElement();

		/**
		 * @brief Create a GUI element with a specific rectangle as a child of root.
		 * @param r Rectangle for the element.
		 * @return Pointer to the created CGUIElement.
		 */
		CGUIElement* createElement(const core::rectf& r);

		/**
		 * @brief Create a GUI element with a specific parent and rectangle.
		 * @param e Parent element.
		 * @param r Rectangle for the element.
		 * @return Pointer to the created CGUIElement.
		 */
		CGUIElement* createElement(CGUIElement* e, const core::rectf& r);

		// ---- Image ----

		/**
		 * @brief Create an image element as a child of root.
		 * @return Pointer to CGUIImage.
		 */
		CGUIImage* createImage();

		/**
		 * @brief Create an image element with a rectangle as a child of root.
		 * @param r Rectangle for the image.
		 * @return Pointer to CGUIImage.
		 */
		CGUIImage* createImage(const core::rectf& r);

		/**
		 * @brief Create an image element with a parent and rectangle.
		 * @param e Parent element.
		 * @param r Rectangle for the image.
		 * @return Pointer to CGUIImage.
		 */
		CGUIImage* createImage(CGUIElement* e, const core::rectf& r);

		// ---- Text ----

		/**
		 * @brief Create a text element with a font as a child of root.
		 * @param font Font for the text.
		 * @return Pointer to CGUIText.
		 */
		CGUIText* createText(IFont* font);

		/**
		 * @brief Create a text element with a rectangle and font as a child of root.
		 * @param r Rectangle.
		 * @param font Font.
		 * @return Pointer to CGUIText.
		 */
		CGUIText* createText(const core::rectf& r, IFont* font);

		/**
		 * @brief Create a text element with a parent and font.
		 * @param e Parent element.
		 * @param font Font.
		 * @return Pointer to CGUIText.
		 */
		CGUIText* createText(CGUIElement* e, IFont* font);

		/**
		 * @brief Create a text element with a parent, rectangle and font.
		 * @param e Parent element.
		 * @param r Rectangle.
		 * @param font Font.
		 * @return Pointer to CGUIText.
		 */
		CGUIText* createText(CGUIElement* e, const core::rectf& r, IFont* font);

		// ---- Sprite ----

		/**
		 * @brief Create a sprite element with a frame as a child of root.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUISprite.
		 */
		CGUISprite* createSprite(SFrame* frame);

		/**
		 * @brief Create a sprite element with a rectangle and frame.
		 * @param r Rectangle.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUISprite.
		 */
		CGUISprite* createSprite(const core::rectf& r, SFrame* frame);

		/**
		 * @brief Create a sprite element with a parent and frame.
		 * @param e Parent element.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUISprite.
		 */
		CGUISprite* createSprite(CGUIElement* e, SFrame* frame);

		/**
		 * @brief Create a sprite element with a parent, rectangle, and frame.
		 * @param e Parent element.
		 * @param r Rectangle.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUISprite.
		 */
		CGUISprite* createSprite(CGUIElement* e, const core::rectf& r, SFrame* frame);

		// ---- Mask ----

		/**
		 * @brief Create a mask element with a rectangle as a child of root.
		 * @param r Rectangle.
		 * @return Pointer to CGUIMask.
		 */
		CGUIMask* createMask(const core::rectf& r);

		/**
		 * @brief Create a mask element with a parent and rectangle.
		 * @param e Parent element.
		 * @param r Rectangle.
		 * @return Pointer to CGUIMask.
		 */
		CGUIMask* createMask(CGUIElement* e, const core::rectf& r);

		// ---- Rect ----

		/**
		 * @brief Create a rectangle element with a color as a child of root.
		 * @param c Color.
		 * @return Pointer to CGUIRect.
		 */
		CGUIRect* createRect(const video::SColor& c);

		/**
		 * @brief Create a rectangle element with a rectangle and color.
		 * @param r Rectangle.
		 * @param c Color.
		 * @return Pointer to CGUIRect.
		 */
		CGUIRect* createRect(const core::rectf& r, const video::SColor& c);

		/**
		 * @brief Create a rectangle element with a parent, rectangle, and color.
		 * @param e Parent element.
		 * @param r Rectangle.
		 * @param c Color.
		 * @return Pointer to CGUIRect.
		 */
		CGUIRect* createRect(CGUIElement* e, const core::rectf& r, const video::SColor& c);

		// ---- Elipse ----

		/**
		 * @brief Create an elipse element with a color as a child of root.
		 * @param c Color.
		 * @return Pointer to CGUIElipse.
		 */
		CGUIElipse* createElipse(const video::SColor& c);

		/**
		 * @brief Create an elipse element with a rectangle and color.
		 * @param r Rectangle.
		 * @param c Color.
		 * @return Pointer to CGUIElipse.
		 */
		CGUIElipse* createElipse(const core::rectf& r, const video::SColor& c);

		/**
		 * @brief Create an elipse element with a parent, rectangle, and color.
		 * @param e Parent element.
		 * @param r Rectangle.
		 * @param c Color.
		 * @return Pointer to CGUIElipse.
		 */
		CGUIElipse* createElipse(CGUIElement* e, const core::rectf& r, const video::SColor& c);

		// ---- Layout ----

		/**
		 * @brief Create a layout element as a child of root.
		 * @return Pointer to CGUILayout.
		 */
		CGUILayout* createLayout();

		/**
		 * @brief Create a layout element with a rectangle.
		 * @param r Rectangle.
		 * @return Pointer to CGUILayout.
		 */
		CGUILayout* createLayout(const core::rectf& r);

		/**
		 * @brief Create a layout element with a parent and rectangle.
		 * @param e Parent element.
		 * @param r Rectangle.
		 * @return Pointer to CGUILayout.
		 */
		CGUILayout* createLayout(CGUIElement* e, const core::rectf& r);

		// ---- Fit Sprite ----

		/**
		 * @brief Create a fit sprite element with a frame as a child of root.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUIFitSprite.
		 */
		CGUIFitSprite* createFitSprite(SFrame* frame);

		/**
		 * @brief Create a fit sprite element with a rectangle and frame.
		 * @param r Rectangle.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUIFitSprite.
		 */
		CGUIFitSprite* createFitSprite(const core::rectf& r, SFrame* frame);

		/**
		 * @brief Create a fit sprite element with a parent and frame.
		 * @param e Parent element.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUIFitSprite.
		 */
		CGUIFitSprite* createFitSprite(CGUIElement* e, SFrame* frame);

		/**
		 * @brief Create a fit sprite element with a parent, rectangle and frame.
		 * @param e Parent element.
		 * @param r Rectangle.
		 * @param frame Sprite frame.
		 * @return Pointer to CGUIFitSprite.
		 */
		CGUIFitSprite* createFitSprite(CGUIElement* e, const core::rectf& r, SFrame* frame);

		// ---- Null element creation (for importer) ----

		/**
		 * @brief Create a null GUI element by type (UTF-8).
		 * @param parent Parent element.
		 * @param type Type name.
		 * @return Pointer to CGUIElement or derived type.
		 */
		virtual CGUIElement* createNullElement(CGUIElement* parent, const char* type);

		/**
		 * @brief Create a null GUI element by type (Unicode).
		 * @param parent Parent element.
		 * @param type Type name (wide char).
		 * @return Pointer to CGUIElement or derived type.
		 */
		virtual CGUIElement* createNullElement(CGUIElement* parent, const wchar_t* type);

		/**
		* @brief Get the world transform matrix for rendering.
		* @return Reference to the matrix.
		*/
		const core::matrix4& getRenderWorldTransform()
		{
			return m_renderWorldTransform;
		}

		/**
		 * @brief Set the world transform matrix for rendering.
		 * @param w Matrix to set.
		 */
		inline void setRenderWorldTransform(core::matrix4& w)
		{
			m_renderWorldTransform = w;
		}

		/**
		 * @brief Get the camera currently used for rendering.
		 * @return Pointer to camera.
		 */
		inline CCamera* getRenderCamera()
		{
			return m_renderCamera;
		}

		/**
		* @brief Find a GUI element by its ID.
		* @param id String ID.
		* @return Pointer to CGUIElement, or NULL if not found.
		*/
		CGUIElement* getGUIByID(const char* id);

		/**
		* @brief Find a GUI element by its path (separated by / or \).
		* @param path String path.
		* @return Pointer to CGUIElement, or NULL if not found.
		*/
		CGUIElement* getGUIByPath(const char* path);

		/**
		 * @brief Find a GUI element by path starting from a specific element.
		 * @param search Starting element.
		 * @param path String path.
		 * @return Pointer to CGUIElement, or NULL if not found.
		 */
		CGUIElement* getGUIByPath(CGUIElement* search, const char* path);

		/**
		 * @brief Find all child elements of a certain type.
		 * @tparam T Type to search for.
		 * @param addThis If true, include the root element.
		 * @return Vector of pointers to found elements of type T.
		 */
		template<typename T>
		std::vector<T*> getElementsInChild(bool addThis);

		/**
		* @brief Find all CGUIText* and update localized text,
		* the event OnLocalize must bind to the localize function
		*/
		void updateLocalizedText();
	};

	template<typename T>
	std::vector<T*> CCanvas::getElementsInChild(bool addThis)
	{
		std::vector<T*> result;
		std::queue<CGUIElement*> queueObjs;

		if (addThis == true)
			queueObjs.push(m_root);
		else
		{
			for (CGUIElement*& obj : m_root->getChilds())
				queueObjs.push(obj);
		}

		while (queueObjs.size() != 0)
		{
			CGUIElement* obj = queueObjs.front();
			queueObjs.pop();

			T* type = dynamic_cast<T*>(obj);
			if (type != NULL)
				result.push_back(type);

			for (CGUIElement*& child : obj->m_childs)
				queueObjs.push(child);
		}

		return result;
	}
}