/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CSprite.h"
#include "CSpriteRenderer.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntity.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CSprite::CSprite() :
		m_data(NULL)
	{

	}

	CSprite::~CSprite()
	{
		if (m_gameObject)
			m_gameObject->getEntity()->removeData<CSpriteDrawData>();
	}

	void CSprite::initComponent()
	{
		m_data = m_gameObject->getEntity()->addData<CSpriteDrawData>();

		m_gameObject->getEntityManager()->addRenderSystem<CSpriteRenderer>();
	}

	void CSprite::updateComponent()
	{

	}

	void CSprite::setFrame(SFrame* frame, float scale, const SColor& color)
	{
		m_data->Frame = frame;
		m_data->Scale = scale;
		m_data->Color = color;
	}

	void CSprite::setCenter(bool b)
	{
		m_data->Center = b;
	}

	void CSprite::setBillboard(bool b)
	{
		m_data->Billboard = b;
	}

	void CSprite::setAutoScaleInViewSpace(bool b)
	{
		m_data->AutoScaleInViewSpace = b;
	}
}