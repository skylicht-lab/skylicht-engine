#ifndef _LINE_DRAW_DATA_
#define _LINE_DRAW_DATA_

#include "Entity/IEntityData.h"

namespace Skylicht
{
	class CLineDrawData : public IEntityData
	{
	public:
		IMeshBuffer* LineBuffer;

	public:
		CLineDrawData();

		virtual ~CLineDrawData();

		void add3DBoxToBuffer(const core::aabbox3d<f32>& box, SColor color);

		void addLineVertexBatch(const core::vector3df& v1, const core::vector3df& v2, const SColor& color);

		void clearBuffer();

		void updateBuffer();
	};
}

#endif