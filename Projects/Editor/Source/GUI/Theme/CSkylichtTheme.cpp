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

#include "pch.h"
#include "CSkylichtTheme.h"
#include "GUI/CGUIContext.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtTheme::CSkylichtTheme()
			{
				m_renderer = dynamic_cast<CSkylichtRenderer*>(CRenderer::getRenderer());
				m_graphics = CGraphics2D::getInstance();
				m_materialID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");

				// atlas texture 1024x1024
				m_sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 1024, 1024);

				// add empty
				m_empty = m_sprite->addFrame("empty", "Editor/GUI/empty.png");

				// add system icon
				initSystemIcon();

				// add dock hint icon
				initDockHintIcon();

				// add window
				m_window = m_sprite->addFrame("draw_window", "Editor/GUI/draw_window.png");
				m_windowShadow = m_sprite->addFrame("draw_window_shadow", "Editor/GUI/draw_window_shadow.png");

				// add button
				m_button = m_sprite->addFrame("draw_button", "Editor/GUI/draw_button.png");
				m_buttonShadow = m_sprite->addFrame("draw_button_shadow", "Editor/GUI/draw_button_shadow.png");

				// add tab button
				m_tabButton = m_sprite->addFrame("draw_tab_button", "Editor/GUI/draw_tab_button.png");
				m_tabButtonFocus = m_sprite->addFrame("draw_tab_button_focus", "Editor/GUI/draw_tab_button_focus.png");

				// add scroll bar
				m_scrollbarH = m_sprite->addFrame("draw_scroll_bar_h", "Editor/GUI/draw_scroll_bar_h.png");
				m_scrollbarV = m_sprite->addFrame("draw_scroll_bar_v", "Editor/GUI/draw_scroll_bar_v.png");

				// add text box
				m_textboxShadow = m_sprite->addFrame("draw_textbox_shadow", "Editor/GUI/draw_textbox_shadow.png");
				m_textbox = m_sprite->addFrame("draw_textbox", "Editor/GUI/draw_textbox.png");
				m_textboxBorder = m_sprite->addFrame("draw_textbox_border", "Editor/GUI/draw_textbox_border.png");
				m_textboxButtonLeft = m_sprite->addFrame("draw_textbox_button_left", "Editor/GUI/draw_textbox_button_left.png");
				m_textboxButtonRight = m_sprite->addFrame("draw_textbox_button_right", "Editor/GUI/draw_textbox_button_right.png");
				m_textboxButtonArrowLeft = m_sprite->addFrame("draw_textbox_button_arrow_left", "Editor/GUI/draw_textbox_button_arrow_left.png");
				m_textboxButtonArrowRight = m_sprite->addFrame("draw_textbox_button_arrow_right", "Editor/GUI/draw_textbox_button_arrow_right.png");
				m_textboxButtonArrowDown = m_sprite->addFrame("draw_textbox_button_arrow_down", "Editor/GUI/draw_textbox_button_arrow_down.png");

				// add font text
				m_renderer->initFont(m_sprite);

				// compile texture
				m_sprite->updateTexture();
			}

			CSkylichtTheme::~CSkylichtTheme()
			{
				delete m_sprite;
			}

			void CSkylichtTheme::initDockHintIcon()
			{
				for (int i = 0; i < EDockHintIcon::NumDockIcon; i++)
					m_dockIcon[i] = NULL;

				m_dockIcon[Center] = m_sprite->addFrame("dock_center", "Editor/GUI/dock_target_center.png");

				m_dockIcon[Left] = m_sprite->addFrame("dock_left", "Editor/GUI/dock_canvas_left.png");
				m_dockIcon[Right] = m_sprite->addFrame("dock_right", "Editor/GUI/dock_canvas_right.png");
				m_dockIcon[Top] = m_sprite->addFrame("dock_top", "Editor/GUI/dock_canvas_top.png");
				m_dockIcon[Bottom] = m_sprite->addFrame("dock_bottom", "Editor/GUI/dock_canvas_bottom.png");

				m_dockIcon[TargetLeft] = m_sprite->addFrame("dock_target_left", "Editor/GUI/dock_target_left.png");
				m_dockIcon[TargetRight] = m_sprite->addFrame("dock_target_right", "Editor/GUI/dock_target_right.png");
				m_dockIcon[TargetTop] = m_sprite->addFrame("dock_target_top", "Editor/GUI/dock_target_top.png");
				m_dockIcon[TargetBottom] = m_sprite->addFrame("dock_target_bottom", "Editor/GUI/dock_target_bottom.png");
			}

			void CSkylichtTheme::initSystemIcon()
			{
				for (int i = 0; i < ESystemIcon::NumSystemIcon; i++)
				{
					m_systemIcon16[i] = NULL;
					m_systemIcon32[i] = NULL;
				}

				addSystemIcon(ESystemIcon::Close, "close.png");
				addSystemIcon(ESystemIcon::Quit, "quit.png");
				addSystemIcon(ESystemIcon::Plus, "plus.png");
				addSystemIcon(ESystemIcon::Minus, "minus.png");
				addSystemIcon(ESystemIcon::Maximize, "maximize.png");
				addSystemIcon(ESystemIcon::RestoreDown, "restore_down.png");
				addSystemIcon(ESystemIcon::TriangleLeft, "triangle_left.png");
				addSystemIcon(ESystemIcon::TriangleRight, "triangle_right.png");
				addSystemIcon(ESystemIcon::TriangleUp, "triangle_up.png");
				addSystemIcon(ESystemIcon::TriangleDown, "triangle_down.png");
				addSystemIcon(ESystemIcon::VRight, "v_right.png");
				addSystemIcon(ESystemIcon::VDown, "v_down.png");
				addSystemIcon(ESystemIcon::SmallTriangleRight, "small_triangle_right.png");
				addSystemIcon(ESystemIcon::SmallTriangleDown, "small_triangle_down.png");
				addSystemIcon(ESystemIcon::Question, "question.png");
				addSystemIcon(ESystemIcon::Alert, "alert.png");
				addSystemIcon(ESystemIcon::Error, "error.png");
				addSystemIcon(ESystemIcon::Check, "check.png");
				addSystemIcon(ESystemIcon::CheckBox, "checkbox.png");
				addSystemIcon(ESystemIcon::CheckBoxSelect, "checkbox_select.png");
				addSystemIcon(ESystemIcon::Radio, "radio.png");
				addSystemIcon(ESystemIcon::RadioSelect, "radio_select.png");
				addSystemIcon(ESystemIcon::Add, "add.png");
				addSystemIcon(ESystemIcon::Setting, "setting.png");
				addSystemIcon(ESystemIcon::Adjustment, "adjustment.png");
				addSystemIcon(ESystemIcon::Tool, "tool.png");
				addSystemIcon(ESystemIcon::ZoomIn, "zoom_in.png");
				addSystemIcon(ESystemIcon::ZoomOut, "zoom_out.png");
				addSystemIcon(ESystemIcon::Search, "search.png");
				addSystemIcon(ESystemIcon::FileSearch, "file_search.png");
				addSystemIcon(ESystemIcon::Lock, "lock.png");
				addSystemIcon(ESystemIcon::Unlock, "unlock.png");
				addSystemIcon(ESystemIcon::Visible, "visible.png");
				addSystemIcon(ESystemIcon::Invisible, "invisible.png");
				addSystemIcon(ESystemIcon::Link, "link.png");
				addSystemIcon(ESystemIcon::Unlink, "unlink.png");
				addSystemIcon(ESystemIcon::Import, "import.png");
				addSystemIcon(ESystemIcon::Export, "export.png");
				addSystemIcon(ESystemIcon::Attach, "attach.png");
				addSystemIcon(ESystemIcon::Copy, "copy.png");
				addSystemIcon(ESystemIcon::Paste, "paste.png");
				addSystemIcon(ESystemIcon::Duplicate, "duplicate.png");
				addSystemIcon(ESystemIcon::Bookmark, "bookmark.png");
				addSystemIcon(ESystemIcon::Filter, "filter.png");
				addSystemIcon(ESystemIcon::Home, "home.png");
				addSystemIcon(ESystemIcon::User, "user.png");
				addSystemIcon(ESystemIcon::Refresh, "refresh.png");
				addSystemIcon(ESystemIcon::UpToParent, "up_to_parent.png");
				addSystemIcon(ESystemIcon::Back, "back.png");
				addSystemIcon(ESystemIcon::Next, "next.png");
				addSystemIcon(ESystemIcon::NewFile, "new_file.png");
				addSystemIcon(ESystemIcon::NewFolder, "new_folder.png");
				addSystemIcon(ESystemIcon::Save, "save.png");
				addSystemIcon(ESystemIcon::Open, "open.png");
				addSystemIcon(ESystemIcon::Trash, "trash.png");
				addSystemIcon(ESystemIcon::Collection, "collection.png");
				addSystemIcon(ESystemIcon::AddCollection, "new_collection.png");
				addSystemIcon(ESystemIcon::Res3D, "res_3d.png");
				addSystemIcon(ESystemIcon::ResImage, "res_image.png");
				addSystemIcon(ESystemIcon::ResMusic, "res_music.png");
				addSystemIcon(ESystemIcon::ResFile, "res_file.png");
				addSystemIcon(ESystemIcon::ResFolder, "res_folder.png");
				addSystemIcon(ESystemIcon::ResVideo, "res_video.png");
				addSystemIcon(ESystemIcon::SortAZ, "sort_az.png");
				addSystemIcon(ESystemIcon::SortExt, "sort_ext.png");
				addSystemIcon(ESystemIcon::SortTime, "sort_time.png");
				addSystemIcon(ESystemIcon::SortDown, "sort_down.png");
				addSystemIcon(ESystemIcon::SortUp, "sort_up.png");
				addSystemIcon(ESystemIcon::PickColor, "pick_color.png");
				addSystemIcon(ESystemIcon::Info, "info.png");
				addSystemIcon(ESystemIcon::Guide, "guide.png");
				addSystemIcon(ESystemIcon::Tree, "tree.png");
				addSystemIcon(ESystemIcon::DragRow, "drag_row.png");
				addSystemIcon(ESystemIcon::Window, "window.png");
				addSystemIcon(ESystemIcon::Windows, "windows.png");
				addSystemIcon(ESystemIcon::Pin, "pin.png");
				addSystemIcon(ESystemIcon::Unpin, "unpin.png");
				addSystemIcon(ESystemIcon::Web, "web.png");
				addSystemIcon(ESystemIcon::Folder, "folder.png");
				addSystemIcon(ESystemIcon::OpenFolder, "open-folder.png");
				addSystemIcon(ESystemIcon::File, "file.png");
				addSystemIcon(ESystemIcon::FileDocument, "file-document.png");
				addSystemIcon(ESystemIcon::FileImage, "file-image.png");

				addEditorIcon(ESystemIcon::Camera, "camera.png");
				addEditorIcon(ESystemIcon::Ortho, "camera_ortho.png");
				addEditorIcon(ESystemIcon::Perspective, "camera_perspective.png");
				addEditorIcon(ESystemIcon::Poly, "draw_poly.png");
				addEditorIcon(ESystemIcon::Spline, "draw_spline.png");
				addEditorIcon(ESystemIcon::LightAreaCircle, "light_area_circle.png");
				addEditorIcon(ESystemIcon::LightAreaRect, "light_area_rect.png");
				addEditorIcon(ESystemIcon::LightDirection, "light_direction.png");
				addEditorIcon(ESystemIcon::LightPoint, "light_point.png");
				addEditorIcon(ESystemIcon::LightSpot, "light_spot.png");
				addEditorIcon(ESystemIcon::ObjectBox, "obj_box.png");
				addEditorIcon(ESystemIcon::ObjectCamera, "obj_camera.png");
				addEditorIcon(ESystemIcon::ObjectCapsule, "obj_capsule.png");
				addEditorIcon(ESystemIcon::ObjectCircle, "obj_circle.png");
				addEditorIcon(ESystemIcon::ObjectCone, "obj_cone.png");
				addEditorIcon(ESystemIcon::ObjectCylinder, "obj_cylinder.png");
				addEditorIcon(ESystemIcon::ObjectIcosphere, "obj_icosphere.png");
				addEditorIcon(ESystemIcon::ObjectLight, "obj_light.png");
				addEditorIcon(ESystemIcon::ObjectMonkey, "obj_monkey.png");
				addEditorIcon(ESystemIcon::ObjectPlane, "obj_plane.png");
				addEditorIcon(ESystemIcon::ObjectRect, "obj_rect.png");
				addEditorIcon(ESystemIcon::ObjectSphere, "obj_sphere.png");
				addEditorIcon(ESystemIcon::ObjectTorus, "obj_torus.png");
				addEditorIcon(ESystemIcon::Move, "tool_move.png");
				addEditorIcon(ESystemIcon::Rotate, "tool_rotate.png");
				addEditorIcon(ESystemIcon::Scale, "tool_scale.png");
				addEditorIcon(ESystemIcon::ViewSelect, "view_select.png");
				addEditorIcon(ESystemIcon::ViewHand, "view_hand.png");
				addEditorIcon(ESystemIcon::Axis, "axis.png");
				addEditorIcon(ESystemIcon::OriginXYZ, "origin_xyz.png");
				addEditorIcon(ESystemIcon::Snap, "snap.png");
				addEditorIcon(ESystemIcon::MagnetOff, "magnet_off.png");
				addEditorIcon(ESystemIcon::MagnetOn, "magnet_on.png");
				addEditorIcon(ESystemIcon::AppIcon, "app_icon.png");

			}

			void CSkylichtTheme::addSystemIcon(ESystemIcon type, const char* name)
			{
				char path[512];
				std::string nameNoExt = CPath::getFileNameNoExt(name);
				if (nameNoExt.empty() == true)
					return;

				sprintf(path, "Editor/Icon/System/16/%s", name);
				m_systemIcon16[type] = m_sprite->addFrame(nameNoExt.c_str(), path);

				sprintf(path, "Editor/Icon/System/32/%s", name);
				m_systemIcon32[type] = m_sprite->addFrame(nameNoExt.c_str(), path);
			}

			void CSkylichtTheme::addEditorIcon(ESystemIcon type, const char* name)
			{
				char path[512];
				std::string nameNoExt = CPath::getFileNameNoExt(name);
				if (nameNoExt.empty() == true)
					return;

				sprintf(path, "Editor/Icon/Editor/16/%s", name);
				m_systemIcon16[type] = m_sprite->addFrame(nameNoExt.c_str(), path);

				sprintf(path, "Editor/Icon/Editor/32/%s", name);
				m_systemIcon32[type] = m_sprite->addFrame(nameNoExt.c_str(), path);
			}

			void CSkylichtTheme::drawIcon(const SRect& r, ESystemIcon icon, const SGUIColor& color, bool use32Bit)
			{
				SFrame* frame = NULL;

				if (use32Bit == true)
					frame = m_systemIcon32[icon];
				else
					frame = m_systemIcon16[icon];

				if (frame == NULL)
					return;

				m_graphics->addFrameBatch(frame, getColor(color), m_renderer->getWorldTransform(), m_materialID);
			}

			void CSkylichtTheme::drawDockHintIcon(const SRect& r, EDockHintIcon icon, const SGUIColor& color)
			{
				SFrame* frame = m_dockIcon[icon];

				if (frame == NULL)
					return;

				m_graphics->addFrameBatch(frame, getColor(color), m_renderer->getWorldTransform(), m_materialID);
			}

			void CSkylichtTheme::drawGUIModule(SFrame* frame, const SRect& rect, const SGUIColor& color, float left, float top, float right, float bottom, float cornerRadius)
			{
				SModuleOffset* module = &frame->ModuleOffset[0];

				core::rectf r = getRect(rect);

				r.UpperLeftCorner.X = r.UpperLeftCorner.X - left + cornerRadius;
				r.UpperLeftCorner.Y = r.UpperLeftCorner.Y - top + cornerRadius;
				r.LowerRightCorner.X = r.LowerRightCorner.X + (module->Module->W - right - cornerRadius);
				r.LowerRightCorner.Y = r.LowerRightCorner.Y + (module->Module->H - bottom - cornerRadius);

				m_graphics->addModuleBatch(
					module,
					getColor(color),
					m_renderer->getWorldTransform(),
					r,
					left, right, top, bottom,
					m_materialID);
			}

			void CSkylichtTheme::drawWindow(const SRect& rect, bool isFocussed)
			{
				float left = 11.0f;
				float top = 11.0f;
				float right = 55.0f;
				float bottom = 55.0f;
				float radius = 3.0f;

				drawGUIModule(m_window, rect, CThemeConfig::WindowBackgroundColor, left, top, right, bottom, radius);
			}

			void CSkylichtTheme::drawWindowShadow(const SRect& rect)
			{
				float left = 11.0f;
				float top = 11.0f;
				float right = 55.0f;
				float bottom = 55.0f;
				float radius = 3.0f;

				drawGUIModule(m_windowShadow, rect, CThemeConfig::White, left, top, right, bottom, radius);
			}

			void CSkylichtTheme::drawButtonShadow(const SRect& rect)
			{
				float left = 6.0f;
				float top = 6.0f;
				float right = 18.0f;
				float bottom = 18.0f;
				float radius = 3.0f;

				drawGUIModule(m_buttonShadow, rect, CThemeConfig::White, left, top, right, bottom, radius);
			}

			void CSkylichtTheme::drawButton(const SRect& rect, const SGUIColor& color)
			{
				float left = 6.0f;
				float top = 6.0f;
				float right = 18.0f;
				float bottom = 18.0f;
				float radius = 3.0f;

				drawGUIModule(m_button, rect, color, left, top, right, bottom, radius);
			}

			void CSkylichtTheme::drawTextBox(const SRect& rect, const SGUIColor& color)
			{
				float left = 5.0f;
				float top = 4.0f;
				float right = 19.0f;
				float bottom = 18.0f;
				float radius = 3.0f;

				drawGUIModule(m_textboxShadow, rect, CThemeConfig::White, left, top, right, bottom, radius);
				drawGUIModule(m_textbox, rect, color, left, top, right, bottom, radius);
			}

			void CSkylichtTheme::drawTextBoxButton(const SRect& rect, const SGUIColor& color, const SGUIColor& iconColor, bool left, bool right)
			{
				if (left == true)
				{
					SModuleOffset* module = &m_textboxButtonLeft->ModuleOffset[0];

					core::rectf r = getRect(rect);
					r.UpperLeftCorner.X = r.UpperLeftCorner.X;
					r.UpperLeftCorner.Y = r.UpperLeftCorner.Y + 1;
					r.LowerRightCorner.X = r.UpperLeftCorner.X + module->Module->W;
					r.LowerRightCorner.Y = r.UpperLeftCorner.Y + module->Module->H;

					m_graphics->addModuleBatch(
						module,
						getColor(color),
						m_renderer->getWorldTransform(),
						r,
						m_materialID);

					module = &m_textboxButtonArrowLeft->ModuleOffset[0];
					m_graphics->addModuleBatch(
						module,
						getColor(iconColor),
						m_renderer->getWorldTransform(),
						r,
						m_materialID);
				}

				if (right == true)
				{
					SModuleOffset* module = &m_textboxButtonRight->ModuleOffset[0];

					core::rectf r = getRect(rect);
					r.UpperLeftCorner.X = r.LowerRightCorner.X - module->Module->W;
					r.UpperLeftCorner.Y = r.UpperLeftCorner.Y + 1;
					r.LowerRightCorner.X = r.UpperLeftCorner.X + module->Module->W;
					r.LowerRightCorner.Y = r.UpperLeftCorner.Y + module->Module->H;

					m_graphics->addModuleBatch(
						module,
						getColor(color),
						m_renderer->getWorldTransform(),
						r,
						m_materialID);

					module = &m_textboxButtonArrowRight->ModuleOffset[0];
					m_graphics->addModuleBatch(
						module,
						getColor(iconColor),
						m_renderer->getWorldTransform(),
						r,
						m_materialID);
				}
			}

			void CSkylichtTheme::drawComboBoxButton(const SRect& rect, const SGUIColor& color, const SGUIColor& iconColor)
			{
				SModuleOffset* module = &m_textboxButtonRight->ModuleOffset[0];

				core::rectf r = getRect(rect);
				r.UpperLeftCorner.X = r.LowerRightCorner.X - module->Module->W;
				r.UpperLeftCorner.Y = r.UpperLeftCorner.Y + 1;
				r.LowerRightCorner.X = r.UpperLeftCorner.X + module->Module->W;
				r.LowerRightCorner.Y = r.UpperLeftCorner.Y + module->Module->H;

				m_graphics->addModuleBatch(
					module,
					getColor(color),
					m_renderer->getWorldTransform(),
					r,
					m_materialID);

				module = &m_textboxButtonArrowDown->ModuleOffset[0];
				m_graphics->addModuleBatch(
					module,
					getColor(iconColor),
					m_renderer->getWorldTransform(),
					r,
					m_materialID);
			}

			void CSkylichtTheme::drawTextBoxBorder(const SRect& rect, const SGUIColor& color)
			{
				float left = 5.0f;
				float top = 4.0f;
				float right = 19.0f;
				float bottom = 18.0f;
				float radius = 3.0f;

				drawGUIModule(m_textboxBorder, rect, color, left, top, right, bottom, radius);
			}

			void CSkylichtTheme::drawTabButton(const SRect& rect, const SGUIColor& color, const SGUIColor& focusColor, bool focus)
			{
				float left = 4.0f;
				float top = 4.0f;
				float right = 18.0f;
				float bottom = 18.0f;
				float radius = 3.0f;

				drawGUIModule(m_tabButton, rect, color, left, top, right, bottom, radius);

				if (focus)
					drawGUIModule(m_tabButtonFocus, rect, focusColor, left, top, right, bottom, radius);
			}

			void CSkylichtTheme::drawScrollbar(const SRect& rect, const SGUIColor& color, bool isHorizontal)
			{
				float left = 4.0f;
				float top = 4.0f;
				float right = 4.0f;
				float bottom = 8.0f;
				float radius = 3.0f;
				SFrame* frame = m_scrollbarV;

				if (isHorizontal == true)
				{
					left = 4.0f;
					top = 4.0f;
					right = 8.0f;
					bottom = 4.0f;
					radius = 3.0f;
					frame = m_scrollbarH;
				}

				drawGUIModule(frame, rect, color, left, top, right, bottom, radius);
			}

			core::rectf CSkylichtTheme::getRect(const SRect& rect)
			{
				return core::rectf(rect.X, rect.Y, rect.X + rect.Width, rect.Y + rect.Height);
			}

			video::SColor CSkylichtTheme::getColor(const SGUIColor& color)
			{
				return SColor(color.A, color.R, color.G, color.B);
			}
		}
	}
}