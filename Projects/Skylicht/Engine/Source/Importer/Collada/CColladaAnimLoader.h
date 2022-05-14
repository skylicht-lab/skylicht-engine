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

#include "Importer/IAnimationImporter.h"

namespace Skylicht
{
	class CAnimationClip;

	struct SEntityAnim;
	struct SNodeParam;

	class CColladaAnimLoader : public IAnimationImporter
	{
	protected:
		bool m_zUp;
		bool m_flipOx;

		std::string m_unit;
		float m_unitScale;
		core::matrix4 m_unitScaleMatrix;

		std::vector<CAnimationClip*> m_clips;
		std::map<std::string, SEntityAnim*> m_nodeAnim;

		SNodeParam* m_colladaRoot;
		std::vector<SNodeParam*> m_listNode;

	public:
		CColladaAnimLoader();

		virtual ~CColladaAnimLoader();

		bool loadAnimation(const char* resource, CAnimationClip* output);

	protected:
		void constructAnimation(const char* fileName, CAnimationClip* output);

		void clearData();

		void parseUnit(io::IXMLReader* xmlRead);

		void parseAnimationNode(io::IXMLReader* xmlRead);

		void parseAnimationSourceNode(io::IXMLReader* xmlRead);

		void parseDefaultValuePosition(io::IXMLReader* xmlRead, float* x, float* y, float* z);

		void parseDefaultValueRotate(io::IXMLReader* xmlRead, float* x, float* y, float* z, float* angle);

		void parseSceneNode(io::IXMLReader* xmlRead);

		SNodeParam* parseNode(io::IXMLReader* xmlRead, SNodeParam* parent);

		SNodeParam* getNode(const std::string& nodeName);
	};
}