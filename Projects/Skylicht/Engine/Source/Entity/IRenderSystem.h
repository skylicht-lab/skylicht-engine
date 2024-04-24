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

#include "CEntity.h"
#include "IEntitySystem.h"
#include "RenderPipeline/IRenderPipeline.h"

namespace Skylicht
{
	class CEntityManager;

	class SKYLICHT_API IRenderSystem : public IEntitySystem
	{
	public:
		enum ERenderPass
		{
			Sky = 0,
			Opaque,
			Transparent,
			Effect,
		};

	protected:
		IRenderPipeline::ERenderPipelineType m_pipelineType;

		ERenderPass m_renderPass;
		int m_sortingPriority;

	public:
		IRenderSystem() :
			m_pipelineType(IRenderPipeline::Forwarder),
			m_renderPass(Opaque),
			m_sortingPriority(0)
		{
		}

		virtual ~IRenderSystem()
		{
		}

		virtual bool isRenderSystem()
		{
			return true;
		}

		IRenderPipeline::ERenderPipelineType getPipelineType()
		{
			return m_pipelineType;
		}

		virtual void render(CEntityManager* entityManager) = 0;

		virtual void renderTransparent(CEntityManager* entityManager)
		{

		}

		virtual void renderEmission(CEntityManager* entityManager)
		{

		}

		virtual void postRender(CEntityManager* entityManager)
		{

		}

		inline ERenderPass getRenderPass()
		{
			return m_renderPass;
		}

		inline int getSortingPriority()
		{
			return m_sortingPriority;
		}

		inline void setSortingPriority(int s)
		{
			m_sortingPriority = s;
		}
	};
}