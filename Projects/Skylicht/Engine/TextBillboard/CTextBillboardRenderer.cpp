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
#include "CTextBillboardRenderer.h"

#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CTextBillboardRenderer::CTextBillboardRenderer()
	{

	}

	CTextBillboardRenderer::~CTextBillboardRenderer()
	{

	}

	void CTextBillboardRenderer::beginQuery(CEntityManager* entityManager)
	{

	}

	void CTextBillboardRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CTextBillboardRenderer::init(CEntityManager* entityManager)
	{

	}

	void CTextBillboardRenderer::update(CEntityManager* entityManager)
	{

	}

	void CTextBillboardRenderer::render(CEntityManager* entityManager)
	{

	}

	void CTextBillboardRenderer::postRender(CEntityManager* entityManager)
	{
		CTextBillboardManager* textMgr = CTextBillboardManager::getInstance();

		IVideoDriver* driver = getVideoDriver();

		float viewportW = textMgr->getViewportWidth();
		float viewportH = textMgr->getViewportHeight();

		if (viewportW <= 0.0f || viewportH <= 0.0f)
		{
			core::recti viewport = driver->getViewPort();
			viewportW = (float)viewport.getWidth();
			viewportH = (float)viewport.getHeight();
		}

		if (viewportW <= 0.0f || viewportH <= 0.0f)
			return;

		beginRender2D(viewportW, viewportH);

		CCamera* camera = entityManager->getCamera();

		float x, y;
		float transformedPos[4];

		core::dimension2d<f32> dim(
			(f32)viewportW * 0.5f,
			(f32)viewportH * 0.5f
		);

		core::matrix4 trans = camera->getProjectionMatrix();
		trans *= camera->getViewMatrix();

		const core::array<CRenderTextData*>& texts = textMgr->getRenderTexts();

		for (u32 i = 0, n = texts.size(); i < n; i++)
		{
			CRenderTextData* text = texts[i];
			if (!text->Visible)
				continue;

			std::vector<std::vector<SModuleOffset*>>& modules = text->getModules();
			if (modules.size() == 0)
				continue;

			if (modules.size() == 1 && modules[0].size() == 0)
				continue;

			if (text->Type == CRenderTextData::Billboard)
			{
				// convert 3d to 2d
				transformedPos[0] = text->Position.X;
				transformedPos[1] = text->Position.Y;
				transformedPos[2] = text->Position.Z;
				transformedPos[3] = 1.0f;

				trans.multiplyWith1x4Matrix(transformedPos);
				f32 zDiv = transformedPos[3] == 0.0f ? 1.0f : core::reciprocal(transformedPos[3]);
				if (zDiv < 0)
					zDiv = -zDiv;

				x = dim.Width + dim.Width * transformedPos[0] * zDiv;
				y = dim.Height - dim.Height * transformedPos[1] * zDiv;

				if (transformedPos[3] >= 0)
				{
					text->Transform.setTranslation(core::vector3df(x, y, 0.0f));
					if (text->UpdateTransformCallback != nullptr)
						text->UpdateTransformCallback(text);

					renderText(text);
				}
			}
			else
			{
				text->Transform.setTranslation(text->Position);
				if (text->UpdateTransformCallback != nullptr)
					text->UpdateTransformCallback(text);

				renderText(text);
			}
		}

		endRender2D();
	}

	void CTextBillboardRenderer::renderText(CRenderTextData* renderTextData)
	{
		std::vector<std::vector<SModuleOffset*>>& lines = renderTextData->getModules();

		float textHeight = renderTextData->getTextHeight();
		float linePadding = renderTextData->getLinePadding();

		float x = 0.0f;
		float y = 0.0f;

		float renderHeight = ((int)lines.size() * (textHeight + linePadding)) - linePadding;

		if (renderTextData->VAlign == CRenderTextData::Middle)
			y = y - (renderHeight + renderTextData->getTextOffsetY()) / 2;
		else if (renderTextData->VAlign == CRenderTextData::Bottom)
			y = y - renderHeight;

		for (int i = 0, n = (int)lines.size(); i < n; i++)
		{
			// render text
			renderText(x, y, renderTextData, lines[i], i);

			// new line
			x = 0.0f;
			y += (textHeight + linePadding);
		}
	}

	void CTextBillboardRenderer::renderText(float x, float y, CRenderTextData* renderTextData, std::vector<SModuleOffset*>& modules, int line)
	{
		int numCharacter = (int)modules.size();

		float stringWidth = 0;
		float charPadding = renderTextData->getSpacePadding();

		for (int i = 0; i < numCharacter; i++)
		{
			SModuleOffset* moduleOffset = modules[i];
			stringWidth = stringWidth + moduleOffset->XAdvance + charPadding;
		}

		if (renderTextData->HAlign == CRenderTextData::Center)
			x = x - stringWidth / 2;
		else if (renderTextData->HAlign == CRenderTextData::Right)
			x = x - stringWidth;

		CGraphics2D* g = CGraphics2D::getInstance();

		for (int i = 0; i < numCharacter; i++)
		{
			SModuleOffset* moduleOffset = modules[i];

			g->addModuleBatch(moduleOffset,
				renderTextData->Color,
				renderTextData->Transform,
				x, y,
				renderTextData->ShaderID,
				renderTextData->Material);

			x = x + moduleOffset->XAdvance + charPadding;
		}
	}
}