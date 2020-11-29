#include "pch.h"
#include "SkylichtEngine.h"
#include "CButton.h"

CButton::CButton(CGUIElement *element, SFrame* frame, const char *label, CGlyphFont *font, const SColor& textColor) :
	m_frame(frame),
	m_label(label),
	m_textColor(textColor),
	m_element(element),
	m_mouseHold(false)
{
	CCanvas *canvas = element->getCanvas();
	m_backround = canvas->createSprite(element, frame);

	m_text = canvas->createText(element, font);
	m_text->setText(m_label.c_str());
	m_text->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
	m_text->setColor(m_textColor);

	CEventManager::getInstance()->registerEvent("CButton", this);
}

CButton::CButton(CGUIElement *element, SFrame* frame) :
	m_frame(frame),
	m_text(NULL),
	m_textColor(255, 255, 255, 255),
	m_element(element),
	m_mouseHold(false)
{
	CCanvas *canvas = element->getCanvas();
	m_backround = canvas->createSprite(element, frame);

	CEventManager::getInstance()->registerEvent("CButton", this);
}

CButton::~CButton()
{
	CEventManager::getInstance()->unRegisterEvent(this);
}

bool CButton::OnEvent(const SEvent& event)
{
	if (m_element->isVisible() == false)
		return false;

	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		// get current canvas
		CCanvas *canvas = m_element->getCanvas();

		// get last camera, that render this button
		CCamera *camera = canvas->getRenderCamera();
		if (camera == NULL)
			return false;

		const core::matrix4& world = canvas->getRenderWorldTransform();
		const core::matrix4& elementTranform = m_element->getAbsoluteTransform();

		// real world transform
		core::matrix4 worldElementTransform = world * elementTranform;

		core::rectf r = m_element->getRect();

		core::vector3df p[4];
		core::vector3df p2d[4];

		p[0].set(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, 0.0f);
		p[1].set(r.LowerRightCorner.X, r.UpperLeftCorner.Y, 0.0f);
		p[2].set(r.UpperLeftCorner.X, r.LowerRightCorner.Y, 0.0f);
		p[3].set(r.LowerRightCorner.X, r.LowerRightCorner.Y, 0.0f);

		for (int i = 0; i < 4; i++)
		{
			// get real 3d position
			worldElementTransform.transformVect(p[i]);

			// project to 2d screen
			CProjective::getScreenCoordinatesFrom3DPosition(camera, p[i], p2d[i].X, p2d[i].Y);
			p2d[i].Z = 0.0f;
		}

		// hit test
		core::triangle3df t1(p2d[0], p2d[1], p2d[2]);
		core::triangle3df t2(p2d[2], p2d[1], p2d[3]);
		core::vector3df mousePos(
			(f32)event.MouseInput.X,
			(f32)event.MouseInput.Y,
			0.0f
		);

		bool mouseOver = false;
		if (t1.isPointInside(mousePos) == true || t2.isPointInside(mousePos) == true)
			mouseOver = true;

		if (m_text != NULL)
		{
			if (mouseOver == true)
			{
				SColor c = m_textColor;
				c.setAlpha(200);
				m_text->setColor(c);
			}
			else
			{
				m_text->setColor(m_textColor);
			}
		}
		else
		{
			if (mouseOver == true)
			{
				SColor c = m_textColor;
				c.setAlpha(200);
				m_backround->setColor(c);
			}
			else
			{
				m_backround->setColor(m_textColor);
			}
		}

		if (mouseOver == true)
		{
			if (event.MouseInput.isLeftPressed() == true)
			{
				SColor bgColor(230, 255, 255, 255);
				m_backround->setColor(bgColor);

				m_mouseHold = true;
			}
		}
		else
		{
			m_backround->setColor(SColor(255, 255, 255, 255));
			m_mouseHold = false;
		}

		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			// reset color
			m_backround->setColor(SColor(255, 255, 255, 255));

			// call event
			if (mouseOver == true)
			{
				if (OnClick != nullptr)
					OnClick();
			}

			m_mouseHold = false;
		}

	}

	return false;
}