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

#include "pch.h"
#include "CGUIElement.h"
#include "Utils/CStringImp.h"
#include "Utils/CRandomID.h"

#include "Graphics2D/CCanvas.h"
#include "Graphics2D/CGraphics2D.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	CGUIElement::CGUIElement(CCanvas* canvas, CGUIElement* parent) :
		m_canvas(canvas),
		m_visible(true),
		m_parent(NULL),
		m_mask(NULL),
		m_drawBorder(false),
		m_enableMaterial(false),
		m_materialId(0),
		m_applyCurrentMask(NULL)
	{
		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		m_entity = entityPrefab->createEntity();
		m_entity->setID(CRandomID::generate().c_str());
		entityPrefab->addTransformData(m_entity, NULL, core::IdentityMatrix, "");

		m_guiTransform = m_entity->addData<CGUITransformData>();
		m_guiAlign = m_entity->addData<CGUIAlignData>();
		m_renderData = m_entity->addData<CGUIRenderData>();
		m_transform = GET_ENTITY_DATA(m_entity, CWorldTransformData);

		setParent(parent);
		setRect(parent->getRect());
	}

	CGUIElement::CGUIElement(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
		m_canvas(canvas),
		m_visible(true),
		m_parent(NULL),
		m_mask(NULL),
		m_drawBorder(false),
		m_enableMaterial(false),
		m_materialId(0),
		m_applyCurrentMask(NULL)
	{
		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		m_entity = entityPrefab->createEntity();
		m_entity->setID(CRandomID::generate().c_str());
		entityPrefab->addTransformData(m_entity, NULL, core::IdentityMatrix, "");

		m_guiTransform = m_entity->addData<CGUITransformData>();
		m_guiAlign = m_entity->addData<CGUIAlignData>();
		m_renderData = m_entity->addData<CGUIRenderData>();
		m_transform = GET_ENTITY_DATA(m_entity, CWorldTransformData);

		setParent(parent);
		setRect(rect);
		setName("No name");
	}

	CGUIElement::~CGUIElement()
	{
		removeAllChilds();

		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		entityPrefab->removeEntity(m_entity);
	}

	void CGUIElement::setName(const wchar_t* name)
	{
		m_transform->Name = CStringImp::convertUnicodeToUTF8(name);
	}

	std::wstring CGUIElement::getNameW()
	{
		return CStringImp::convertUTF8ToUnicode(m_transform->Name.c_str());
	}

	void CGUIElement::setParent(CGUIElement* parent)
	{
		if (m_parent)
		{
			m_parent->removeChild(this);
			m_parent->notifyChanged();
		}

		m_parent = parent;
		if (m_parent)
			m_parent->notifyChanged();

		notifyChanged();

		CEntityPrefab* entityMgr = m_canvas->getEntityManager();

		if (m_parent)
		{
			m_parent->m_childs.push_back(this);

			entityMgr->changeParent(m_entity, m_parent->m_entity);

			CGUITransformData* t = GET_ENTITY_DATA(m_entity, CGUITransformData);
			t->Parent = GET_ENTITY_DATA(m_parent->m_entity, CGUITransformData);

			CWorldTransformData* w = GET_ENTITY_DATA(m_entity, CWorldTransformData);
			w->Parent = GET_ENTITY_DATA(m_parent->m_entity, CWorldTransformData);

			CGUILayoutData* parentLayout = GET_ENTITY_DATA(m_parent->m_entity, CGUILayoutData);
			if (parentLayout != NULL)
			{
				CGUIChildLayoutData* childLayout = m_entity->addData<CGUIChildLayoutData>();
				childLayout->Parent = parentLayout;
			}
		}
		else
		{
			entityMgr->changeParent(m_entity, NULL);

			CGUITransformData* t = GET_ENTITY_DATA(m_entity, CGUITransformData);
			t->Parent = NULL;

			CWorldTransformData* w = GET_ENTITY_DATA(m_entity, CWorldTransformData);
			w->Parent = NULL;

			CGUIChildLayoutData* l = GET_ENTITY_DATA(m_entity, CGUIChildLayoutData);
			if (l != NULL)
			{
				m_entity->removeData<CGUIChildLayoutData>();
			}
		}
	}

	bool CGUIElement::removeChild(CGUIElement* child)
	{
		for (int i = 0, n = (int)m_childs.size(); i < n; i++)
		{
			if (m_childs[i] == child)
			{
				m_childs.erase(m_childs.begin() + i);
				return true;
			}
		}
		return false;
	}

	void CGUIElement::remove()
	{
		if (m_parent)
			m_parent->removeChild(this);

		delete this;
	}

	void CGUIElement::removeAllChilds()
	{
		for (CGUIElement* child : m_childs)
			delete child;

		m_childs.clear();
	}

	const core::rectf CGUIElement::getNativeRect()
	{
		return getRect();
	}

	void CGUIElement::update(CCamera* camera)
	{

	}

	void CGUIElement::render(CCamera* camera)
	{
		bool drawBorder = m_drawBorder;
		SColor borderColor(100, 255, 255, 255);

		if (m_canvas->IsInEditor)
		{
			drawBorder = true;
			if (m_drawBorder)
				borderColor.set(255, 0, 255, 0);
		}

		if (drawBorder)
		{
			core::rectf r = getRect();

			float z = 0.0f;

			core::vector3df topLeft(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, z);
			core::vector3df topRight(r.LowerRightCorner.X, r.UpperLeftCorner.Y, z);
			core::vector3df bottomLeft(r.UpperLeftCorner.X, r.LowerRightCorner.Y, z);
			core::vector3df bottomRight(r.LowerRightCorner.X, r.LowerRightCorner.Y, z);

			m_transform->World.transformVect(topLeft);
			m_transform->World.transformVect(topRight);
			m_transform->World.transformVect(bottomLeft);
			m_transform->World.transformVect(bottomRight);

			std::vector<core::vector2df> lines;
			lines.push_back(core::vector2df(topLeft.X, topLeft.Y));
			lines.push_back(core::vector2df(topRight.X, topRight.Y));
			lines.push_back(core::vector2df(bottomRight.X, bottomRight.Y));
			lines.push_back(core::vector2df(bottomLeft.X, bottomLeft.Y));
			lines.push_back(core::vector2df(topLeft.X, topLeft.Y));

			CGraphics2D::getInstance()->draw2DLines(lines, borderColor);
		}
	}

	void CGUIElement::setMaterialSource(const char* material)
	{
		std::string materialFile = material;

		if (materialFile != m_materialFile)
		{
			m_materialFile = materialFile;

			if (materialFile.empty())
			{
				m_renderData->Material = NULL;
			}
			else
			{
				std::vector<std::string> textureFolders;

				m_materials = CMaterialManager::getInstance()->loadMaterial(
					m_materialFile.c_str(),
					true,
					textureFolders
				);

				if (m_materials.size() > 0)
				{
					if (m_materialId >= m_materials.size())
						m_materialId = 0;

					m_renderData->Material = m_materials[m_materialId];
				}
				else
				{
					m_renderData->Material = NULL;
				}
			}
		}
	}

	CObjectSerializable* CGUIElement::createSerializable()
	{
		CObjectSerializable* object = new CObjectSerializable(getTypeName().c_str());

		// element data
		object->autoRelease(new CStringProperty(object, "name", getName()));
		object->autoRelease(new CBoolProperty(object, "visible", m_visible));

		// transform
		object->autoRelease(new CVector3Property(object, "position", m_guiTransform->getPosition()));
		object->autoRelease(new CVector3Property(object, "scale", m_guiTransform->getScale()));
		object->autoRelease(new CVector3Property(object, "rotation", m_guiTransform->getRotation()));

		// rect
		object->autoRelease(new CFloatProperty(object, "rectX", m_guiTransform->Rect.UpperLeftCorner.X));
		object->autoRelease(new CFloatProperty(object, "rectY", m_guiTransform->Rect.UpperLeftCorner.Y));
		object->autoRelease(new CFloatProperty(object, "rectW", m_guiTransform->Rect.getWidth()));
		object->autoRelease(new CFloatProperty(object, "rectH", m_guiTransform->Rect.getHeight()));

		// align
		CEnumProperty<EGUIDock>* dock = new CEnumProperty<EGUIDock>(object, "dock", m_guiAlign->Dock);
		dock->addEnumString("NoDock", EGUIDock::NoDock);
		dock->addEnumString("DockLeft", EGUIDock::DockLeft);
		dock->addEnumString("DockRight", EGUIDock::DockRight);
		dock->addEnumString("DockTop", EGUIDock::DockTop);
		dock->addEnumString("DockBottom", EGUIDock::DockBottom);
		dock->addEnumString("DockFill", EGUIDock::DockFill);
		object->autoRelease(dock);

		CEnumProperty<EGUIVerticalAlign>* alignVertical = new CEnumProperty<EGUIVerticalAlign>(object, "alignVertical", m_guiAlign->Vertical);
		alignVertical->addEnumString("Top", EGUIVerticalAlign::Top);
		alignVertical->addEnumString("Middle", EGUIVerticalAlign::Middle);
		alignVertical->addEnumString("Bottom", EGUIVerticalAlign::Bottom);
		object->autoRelease(alignVertical);

		CEnumProperty<EGUIHorizontalAlign>* alignHorizontal = new CEnumProperty<EGUIHorizontalAlign>(object, "alignHorizontal", m_guiAlign->Horizontal);
		alignHorizontal->addEnumString("Left", EGUIHorizontalAlign::Left);
		alignHorizontal->addEnumString("Center", EGUIHorizontalAlign::Center);
		alignHorizontal->addEnumString("Right", EGUIHorizontalAlign::Right);
		object->autoRelease(alignHorizontal);

		// padding
		object->autoRelease(new CFloatProperty(object, "marginLeft", m_guiAlign->Margin.Left));
		object->autoRelease(new CFloatProperty(object, "marginTop", m_guiAlign->Margin.Top));
		object->autoRelease(new CFloatProperty(object, "marginRight", m_guiAlign->Margin.Right));
		object->autoRelease(new CFloatProperty(object, "marginBottom", m_guiAlign->Margin.Bottom));

		// color
		object->autoRelease(new CColorProperty(object, "color", m_renderData->Color));

		// materials
		object->autoRelease(new CIntProperty(object, "materialId", m_materialId, 0, (int)m_materials.size()));

		std::vector<std::string> materialExts = { "xml","mat" };
		object->autoRelease(new CFilePathProperty(object, "material", m_materialFile.c_str(), materialExts));

		return object;
	}

	void CGUIElement::loadSerializable(CObjectSerializable* object)
	{
		// gui data
		setName(object->get("name", std::string("No name")).c_str());
		m_visible = object->get("visible", false);

		// transform
		setPosition(object->get<core::vector3df>("position", core::vector3df()));
		setScale(object->get<core::vector3df>("scale", core::vector3df(1.0f, 1.0f, 1.0f)));
		setRotation(object->get<core::vector3df>("rotation", core::vector3df()));

		// rect
		m_guiTransform->Rect.UpperLeftCorner.X = object->get("rectX", 0.0f);
		m_guiTransform->Rect.UpperLeftCorner.Y = object->get("rectY", 0.0f);

		float w = object->get("rectW", 0.0f);
		float h = object->get("rectH", 0.0f);
		m_guiTransform->Rect.LowerRightCorner = m_guiTransform->Rect.UpperLeftCorner + core::vector2df(w, h);

		// align
		m_guiAlign->Dock = object->get<EGUIDock>("dock", EGUIDock::NoDock);
		m_guiAlign->Vertical = object->get<EGUIVerticalAlign>("alignVertical", EGUIVerticalAlign::Top);
		m_guiAlign->Horizontal = object->get<EGUIHorizontalAlign>("alignHorizontal", EGUIHorizontalAlign::Left);

		// padding
		m_guiAlign->Margin.Left = object->get("marginLeft", 0.0f);
		m_guiAlign->Margin.Top = object->get("marginTop", 0.0f);
		m_guiAlign->Margin.Right = object->get("marginRight", 0.0f);
		m_guiAlign->Margin.Bottom = object->get("marginBottom", 0.0f);

		// color
		m_renderData->Color = object->get("color", SColor(255, 255, 255, 255));

		// materials
		m_materialId = object->get<int>("materialId", 0);
		std::string materialFile = object->get<std::string>("material", "");

		if (materialFile != m_materialFile)
		{
			setMaterialSource(materialFile.c_str());
		}

		notifyChanged();
	}

	void CGUIElement::setMaterialId(int id)
	{
		m_materialId = id;
		if (m_materialId >= m_materials.size())
			m_materialId = 0;

		m_renderData->Material = m_materials[m_materialId];
	}

	void CGUIElement::notifyChanged()
	{
		m_guiTransform->HasChanged = true;
		m_transform->HasChanged = true;
	}

	void CGUIElement::bringToNext(CGUIElement* object, CGUIElement* target, bool behind)
	{
		if (target->getParent() != this)
			return;

		core::matrix4 world = object->getAbsoluteTransform();

		if (object->getParent() != this)
		{
			// set new parent
			object->setParent(this);
		}

		// remove the current position
		{
			std::vector<CGUIElement*>::iterator i = std::find(m_childs.begin(), m_childs.end(), object);
			m_childs.erase(i);
		}

		// insert new position
		std::vector<CGUIElement*>::iterator pos = std::find(m_childs.begin(), m_childs.end(), target);
		if (behind)
			++pos;

		m_childs.insert(pos, object);

		object->setWorldTransform(world);
	}

	void CGUIElement::bringToChild(CGUIElement* object)
	{
		core::matrix4 world = object->getAbsoluteTransform();

		if (object->getParent() != this)
		{
			// set new parent
			object->setParent(this);
		}

		// remove the current position
		{
			std::vector<CGUIElement*>::iterator i = std::find(m_childs.begin(), m_childs.end(), object);
			m_childs.erase(i);
		}

		// insert new position
		m_childs.push_back(object);

		object->setWorldTransform(world);
	}

	void CGUIElement::setWorldTransform(const core::matrix4& world)
	{
		core::matrix4 parentInv;
		if (m_transform->Parent)
		{
			parentInv = m_transform->Parent->World;
			parentInv.makeInverse();
		}

		core::matrix4 relative = parentInv * world;
		setRelativeTransform(relative);
	}

	void CGUIElement::setRelativeTransform(const core::matrix4& mat)
	{
		core::vector3df front(0.0f, 0.0f, 1.0f);
		core::vector3df up(0.0f, 1.0f, 0.0f);

		mat.rotateVect(front);
		mat.rotateVect(up);

		front.normalize();
		up.normalize();

		core::vector3df right = up.crossProduct(front);
		right.normalize();

		core::matrix4 rotationMatrix;
		f32* matData = rotationMatrix.pointer();
		matData[0] = right.X;
		matData[1] = right.Y;
		matData[2] = right.Z;
		matData[3] = 0.0f;

		matData[4] = up.X;
		matData[5] = up.Y;
		matData[6] = up.Z;
		matData[7] = 0.0f;

		matData[8] = front.X;
		matData[9] = front.Y;
		matData[10] = front.Z;
		matData[11] = 0.0f;

		matData[12] = 0.0f;
		matData[13] = 0.0f;
		matData[14] = 0.0f;
		matData[15] = 1.0f;

		core::vector3df rotation;
		core::vector3df position;
		core::vector3df scale;

		core::quaternion q(rotationMatrix);
		q.toEuler(rotation);

		rotation = rotation * core::RADTODEG;
		position = mat.getTranslation();
		scale = mat.getScale();

		setPosition(position);
		setScale(scale);
		setRotation(rotation);
	}

	bool CGUIElement::isChild(CGUIElement* e)
	{
		CGUIElement* p = m_parent;
		while (p != NULL)
		{
			if (p == e)
			{
				return true;
			}
			p = p->m_parent;
		}
		return false;
	}
}
