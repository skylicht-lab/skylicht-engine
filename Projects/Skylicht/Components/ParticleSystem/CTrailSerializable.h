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

#pragma once

#include "Serializable/CArraySerializable.h"

namespace Skylicht
{
	/**
	 * @class CTrailSerializable
	 * @ingroup ParticleSystem
	 * @brief Serialization helper for particle trails.
	 */
	class SKYLICHT_API CTrailSerializable : public CObjectSerializable
	{
	public:
		/** @brief The name of the linked particle group. */
		CStringProperty Name;
		/** @brief Width of the trail ribbon. */
		CFloatProperty Width;
		/** @brief Maximum length of the trail. */
		CFloatProperty Length;
		/** @brief Length of each segment in the ribbon. */
		CFloatProperty SegmentLength;
		/** @brief Whether to immediately destroy the trail when its particle dies. */
		CBoolProperty DestroyWhenParticleDead;
		/** @brief Alpha reduction rate for trails left behind by dead particles. */
		CFloatProperty DeadAlphaReduction;
		/** @brief Path to the trail texture. */
		CImageSourceProperty Texture;
		/** @brief Whether to use a custom material file instead of the default. */
		CBoolProperty UseCustomMaterial;
		/** @brief Path to the custom material file. */
		CFilePathProperty CustomMaterial;
		/** @brief Whether the trail has emission (glow). */
		CBoolProperty Emission;
		/** @brief Intensity of the emission. */
		CFloatProperty EmissionIntensity;

	public:
		CTrailSerializable();

		CTrailSerializable(CObjectSerializable* parent);

		virtual ~CTrailSerializable();

		virtual CObjectSerializable* clone();

		DECLARE_GETTYPENAME(CTrailSerializable)
	};
}