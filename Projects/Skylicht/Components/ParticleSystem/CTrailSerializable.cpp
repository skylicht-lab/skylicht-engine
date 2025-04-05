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
#include "CTrailSerializable.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	SERIALIZABLE_REGISTER(CTrailSerializable);

	CTrailSerializable::CTrailSerializable() :
		CObjectSerializable(getTypeName().c_str()),
		Name(this, "name"),
		Width(this, "width"),
		Length(this, "length"),
		SegmentLength(this, "segmentLength"),
		DestroyWhenParticleDead(this, "destroyWhenParticleDead"),
		DeadAlphaReduction(this, "deadAlphaReduction"),
		Texture(this, "texture"),
		UseCustomMaterial(this, "useCustomMaterial"),
		CustomMaterial(this, "customMaterial", "", CMaterialManager::getMaterialExts()),
		Emission(this, "emission", true),
		EmissionIntensity(this, "emissionIntensity", 1.0f)
	{
		Width.set(1.0f);
		Length.set(1.0f);
		SegmentLength.set(0.5f);
		DestroyWhenParticleDead.set(false);
		DeadAlphaReduction.set(0.01f);
		UseCustomMaterial.set(false);
	}

	CTrailSerializable::CTrailSerializable(CObjectSerializable* parent) :
		CObjectSerializable(getTypeName().c_str(), parent),
		Name(this, "name"),
		Width(this, "width"),
		Length(this, "length"),
		SegmentLength(this, "segmentLength"),
		DestroyWhenParticleDead(this, "destroyWhenParticleDead"),
		DeadAlphaReduction(this, "deadAlphaReduction"),
		Texture(this, "texture"),
		UseCustomMaterial(this, "useCustomMaterial"),
		CustomMaterial(this, "customMaterial", "", CMaterialManager::getMaterialExts())
	{
		Width.set(1.0f);
		Length.set(1.0f);
		SegmentLength.set(0.5f);
		DestroyWhenParticleDead.set(false);
		DeadAlphaReduction.set(0.01f);
		UseCustomMaterial.set(false);
	}

	CTrailSerializable::~CTrailSerializable()
	{

	}

	CObjectSerializable* CTrailSerializable::clone()
	{
		CTrailSerializable* object = new CTrailSerializable();
		copyTo(object);
		return object;
	}
}