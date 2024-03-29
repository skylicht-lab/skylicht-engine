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

// Reference: 
// https://github.com/runevision/LocomotionSystem/blob/master/Assets/Locomotion%20System%20Files/Locomotion%20System/PolarGradientBandInterpolator.cs
// https://www.shadertoy.com/view/XlKXWR


#pragma once

namespace Skylicht
{
	class SKYLICHT_API CGradientBandInterpolation
	{
	public:
		struct SSample
		{
			int Id;
			core::vector3df Vector;
			float Weight;

			SSample()
			{
				Id = 0;
				Weight = 0.0f;
			}
		};

	protected:
		core::array<SSample*> m_samples;

	public:
		CGradientBandInterpolation();

		virtual ~CGradientBandInterpolation();

		float getAngle(const core::vector3df& a, const core::vector3df& b);

		core::vector3df project(const core::vector3df& vector, const core::vector3df& onNormal);

		inline core::array<SSample*>& getSamples()
		{
			return m_samples;
		}

		SSample* addSample(int id, const core::vector3df& vector);

		void removeSample(SSample* sample);

		void clear();

		void sampleWeightsPolar(const core::vector3df& vector);

		void sampleWeightsCartesian(const core::vector3df& vector);
	};
}