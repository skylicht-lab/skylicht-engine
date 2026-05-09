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
	/**
	 * @brief Helper class for blending multiple animations based on a parameter vector.
	 * @ingroup Animation
	 * 
	 * This is typically used for locomotion systems (e.g., blending Idle, Walk, Run based on velocity).
	 * Reference: https://github.com/runevision/LocomotionSystem
	 * 
	 * Example: Locomotion blending
	 * @code
	 * CGradientBandInterpolation blending;
	 * blending.addSample(ID_IDLE, core::vector3df(0, 0, 0));
	 * blending.addSample(ID_WALK, core::vector3df(0, 0, 1.5f));
	 * blending.addSample(ID_RUN,  core::vector3df(0, 0, 5.0f));
	 * 
	 * // Target velocity
	 * core::vector3df velocity(0, 0, 3.0f);
	 * blending.sampleWeightsPolar(velocity);
	 * 
	 * // Apply weights to skeletons
	 * for (auto* sample : blending.getSamples()) {
	 *     animController->getSkeleton(sample->Id)->getTimeline().Weight = sample->Weight;
	 * }
	 * @endcode
	 */
	class SKYLICHT_API CGradientBandInterpolation
	{
	public:
		/**
		 * @brief Represents an animation sample point with its associated weight.
		 */
		struct SSample
		{
			//! User-defined ID for the animation sample.
			int Id;
			
			//! Position vector in parameter space.
			core::vector3df Vector;
			
			//! Calculated blending weight (0 to 1).
			float Weight;

			SSample()
			{
				Id = 0;
				Weight = 0.0f;
			}
		};

	protected:
		//! List of all animation samples.
		core::array<SSample*> m_samples;

	public:
		CGradientBandInterpolation();

		virtual ~CGradientBandInterpolation();

		/**
		 * @brief Gets the list of animation samples.
		 * @return Reference to the samples array.
		 */
		inline core::array<SSample*>& getSamples()
		{
			return m_samples;
		}

		/**
		 * @brief Adds a new animation sample point.
		 * @param id The sample ID.
		 * @param vector The position in parameter space.
		 * @return Pointer to the newly created sample.
		 */
		SSample* addSample(int id, const core::vector3df& vector);

		/**
		 * @brief Removes a specific animation sample.
		 * @param sample Pointer to the sample to remove.
		 */
		void removeSample(SSample* sample);

		/**
		 * @brief Removes all animation samples.
		 */
		void clear();

		/**
		 * @brief Calculates blending weights using polar gradient band interpolation.
		 * 
		 * Best suited for parameters like direction and speed.
		 * @param vector The input parameter vector (e.g., target velocity).
		 */
		void sampleWeightsPolar(const core::vector3df& vector);

		/**
		 * @brief Calculates blending weights using Cartesian gradient band interpolation.
		 * 
		 * Best suited for 2D/3D parameter spaces without directional wrapping.
		 * @param vector The input parameter vector.
		 */
		void sampleWeightsCartesian(const core::vector3df& vector);
	};
}