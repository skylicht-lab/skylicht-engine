/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CSkeletonDrawable.h"
#include "CSpineResource.h"

#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/CCanvas.h"
#include "Graphics2D/GUI/CGUIElement.h"

using namespace Skylicht;

spine::SpineExtension* spine::getDefaultExtension() {
	return new spine::DefaultSpineExtension();
}

namespace spine
{
	CSkeletonDrawable::CSkeletonDrawable(spine::SkeletonData* skeletonData, spine::AnimationStateData* animationStateData) :
		m_skeleton(NULL),
		m_animationState(NULL),
		m_ownsAnimationStateData(false)
	{
		m_skeleton = new spine::Skeleton(skeletonData);

		m_ownsAnimationStateData = animationStateData == NULL;
		if (m_ownsAnimationStateData)
			animationStateData = new spine::AnimationStateData(skeletonData);

		m_animationState = new spine::AnimationState(animationStateData);
	}

	CSkeletonDrawable::~CSkeletonDrawable()
	{
		if (m_ownsAnimationStateData)
			delete m_animationState->getData();

		delete m_animationState;
		delete m_skeleton;
	}

	void CSkeletonDrawable::update(float delta, spine::Physics physics)
	{
		m_animationState->update(delta * 0.001f);
		m_animationState->apply(*m_skeleton);
		m_skeleton->update(delta);
		m_skeleton->updateWorldTransform(physics);
	}

	void CSkeletonDrawable::render(CGUIElement* insideElement)
	{
		if (insideElement == NULL)
			return;

		spine::SkeletonRenderer* renderer = CSpineResource::getRenderer();
		if (renderer == NULL)
			return;

		CCanvas* canvas = insideElement->getCanvas();
		float canvasHeight = canvas->getRootElement()->getHeight();

		// move to center of element
		core::vector3df pos = insideElement->getAbsoluteTransform().getTranslation();
		pos.X = pos.X + insideElement->getWidth() * 0.5f + m_drawOffset.X;
		pos.Y = pos.Y + insideElement->getHeight() * 0.5f + m_drawOffset.Y;

		// set position (flip Y)
		m_skeleton->setPosition(pos.X, canvasHeight - pos.Y);

		// flush the current buffer
		CGraphics2D* graphics = CGraphics2D::getInstance();
		video::SMaterial& material = graphics->getMaterial();
		graphics->flush();

		RenderCommand* command = renderer->render(*m_skeleton);
		while (command)
		{
			IMeshBuffer* meshBuffer = graphics->getCurrentBuffer();

			scene::IVertexBuffer* vtxBuffer = meshBuffer->getVertexBuffer();
			scene::IIndexBuffer* idxBuffer = meshBuffer->getIndexBuffer();

			float* positions = command->positions;
			float* uvs = command->uvs;
			uint32_t* colors = command->colors;

			// alloc vertex buffer
			vtxBuffer->set_used(command->numVertices);
			S3DVertex* vertices = (S3DVertex*)vtxBuffer->getVertices();

			for (int ii = 0; ii < command->numVertices << 1; ii += 2)
			{
				S3DVertex* v = &vertices[ii >> 1];

				v->Pos.X = positions[ii];
				// warning: flip Y
				v->Pos.Y = canvasHeight - positions[ii + 1];
				v->Pos.Z = 0.0f;

				v->TCoords.X = uvs[ii];
				v->TCoords.Y = uvs[ii + 1];

				v->Color.set(colors[ii >> 1]);
			}

			// alloc index buffer
			idxBuffer->set_used(command->numIndices);
			u16* index = (u16*)idxBuffer->getIndices();
			uint16_t* indices = command->indices;

			for (int ii = 0; ii < command->numIndices; ii++)
			{
				index[ii] = indices[ii];
			}

			// texture
			material.setTexture(0, (ITexture*)command->texture);

			// blendmode
			BlendMode blendMode = command->blendMode;
			switch (blendMode)
			{
			case BlendMode_Normal:
				material.MaterialType = CSpineResource::getTextureColorBlend();
				break;
			case BlendMode_Multiply:
				material.MaterialType = CSpineResource::getTextureColorMultiply();
				break;
			case BlendMode_Additive:
				material.MaterialType = CSpineResource::getTextureColorAddtive();
				break;
			case BlendMode_Screen:
				material.MaterialType = CSpineResource::getTextureColorScreen();
				break;
			}

			// draw this command
			graphics->flush();

			command = command->next;
		}
	}
}