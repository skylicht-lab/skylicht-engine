/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "Entity/CEntity.h"
#include "Entity/CEntityHandler.h"

namespace Skylicht
{
	/// @brief Light probes are objects that store SH illumination. These SH values are baked from environmental texture maps, such as a skybox or skydome.
	/// @ingroup Lighting
	/// 
	/// You can use Lightmapper::CLightmapper to bake light into CLightProbes, 
	/// or you can also bake it directly from Skylicht-Editor once you have set up the Sky environment.
	/// 
	/// @image html Lighting/light_probes_0.jpg "The ambient light that shines on objects is influenced by light probes and the environment" width=1200px
	/// 
	/// The baking process recalculates the colors and stores them in CLightProbeData.
	/// This value is then mapped to the nearest objects with CIndirectLightingData.
	/// This SH value will eventually be passed to the Shader during the drawing step.
	/// 
	/// @image html Lighting/light_probes_1.jpg "When the environment changes, you need to rebake the light probes" width=600px
	/// 
	/// @code
	/// // Example code for using Lightmapper to bake lighting to probes
	/// CLightProbes* probesComponent = object->getComponent<CLightProbes>();
	/// ...
	/// ...
	/// std::vector<core::vector3df> positions;
	/// std::vector<Lightmapper::CSH9> probes;
	/// std::vector<core::vector3df> results;
	/// 
	/// CEntityManager* entityMgr = scene->getEntityManager();
	/// if (probesComponent->getPositions(positions) > 0)
	/// {
	/// 	entityMgr->update();
	/// 	Lightmapper::CLightmapper::getInstance()->initBaker(32);
	/// 	Lightmapper::CLightmapper::getInstance()->bakeProbes(
	/// 		positions,
	/// 		probes,
	/// 		bakeCamera,
	/// 		renderPipeline,
	/// 		entityMgr
	/// 	);
	/// 	for (Lightmapper::CSH9& sh : probes)
	/// 	{
	/// 		core::vector3df r[9];
	/// 		sh.copyTo(r);
	/// 		for (int i = 0; i < 9; i++)
	/// 			results.push_back(r[i]);
	/// 	}
	/// 	probesComponent->setSH(results);
	/// }
	/// @endcode
	/// 
	/// @see Lightmapper::CLightmapper
	class SKYLICHT_API CLightProbes : public CEntityHandler
	{
	protected:
		float m_intensity;

	public:
		CLightProbes();

		virtual ~CLightProbes();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		virtual CEntity* spawn();

		CEntity* addLightProbe(const core::vector3df& position);

		int getPositions(std::vector<core::vector3df>& positions);

		void setSH(std::vector<core::vector3df>& sh);

		DECLARE_GETTYPENAME(CLightProbes)
	};
}