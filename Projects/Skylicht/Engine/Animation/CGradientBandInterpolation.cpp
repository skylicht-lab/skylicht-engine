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
#include "CGradientBandInterpolation.h"
#include "Utils/CVector.h"

namespace Skylicht
{
	CGradientBandInterpolation::CGradientBandInterpolation()
	{

	}

	CGradientBandInterpolation::~CGradientBandInterpolation()
	{
		clear();
	}

	CGradientBandInterpolation::SSample* CGradientBandInterpolation::addSample(int id, const core::vector3df& vector)
	{
		CGradientBandInterpolation::SSample* ret = new CGradientBandInterpolation::SSample();
		ret->Id = id;
		ret->Vector = vector;
		m_samples.push_back(ret);
		return ret;
	}

	void CGradientBandInterpolation::removeSample(CGradientBandInterpolation::SSample* sample)
	{
		int i = m_samples.linear_search(sample);
		if (i >= 0)
			m_samples.erase(i);
		delete sample;
	}

	void CGradientBandInterpolation::clear()
	{
		for (u32 i = 0, n = m_samples.size(); i < n; i++)
			delete m_samples[i];
		m_samples.clear();
	}

	void CGradientBandInterpolation::sampleWeightsPolar(const core::vector3df& vector)
	{
		int numMotions = (int)m_samples.size();
		SSample** samples = m_samples.pointer();

		for (int i = 0; i < numMotions; i++)
		{
			float value = 1.0f;

			samples[i]->Weight = 0.0f;

			for (int j = 0; j < numMotions; j++)
			{
				if (i == j)
					continue;

				const core::vector3df& sampleI = samples[i]->Vector;
				const core::vector3df& sampleJ = samples[j]->Vector;

				float iAngle = 0.0f, oAngle = 0.0f;

				core::vector3df outputProj;
				float angleMultiplier = 2.0f;

				if (sampleI.getLength() == 0.0f)
				{
					iAngle = CVector::angle(vector, sampleJ);
					oAngle = 0;
					outputProj = vector;
					angleMultiplier = 1.0f;
				}
				else if (sampleJ.getLength() == 0.0f)
				{
					iAngle = CVector::angle(vector, sampleI);
					oAngle = iAngle;
					outputProj = vector;
					angleMultiplier = 1.0f;
				}
				else
				{
					iAngle = CVector::angle(sampleI, sampleJ);
					if (iAngle > 0)
					{
						if (vector.getLength() == 0.0f)
						{
							oAngle = iAngle;
							outputProj = vector;
						}
						else
						{
							core::vector3df axis = sampleI.crossProduct(sampleJ);
							axis.normalize();

							outputProj = vector - CVector::project(vector, axis); // ProjectOntoPlane

							oAngle = CVector::angle(sampleI, outputProj);
							if (iAngle < core::PI * 0.99f)
							{
								core::vector3df cross = sampleI.crossProduct(outputProj);
								cross.normalize();

								if (cross.dotProduct(axis) < 0)
									oAngle = oAngle * -1.0f;
							}
						}
					}
					else
					{
						outputProj = vector;
						oAngle = 0.0f;
					}
				}

				float t1 = iAngle * core::RADTODEG;
				float t2 = oAngle * core::RADTODEG;

				float magI = sampleI.getLength();
				float magJ = sampleJ.getLength();
				float magO = outputProj.getLength();
				float avgMag = (magI + magJ) / 2.0f;

				magI /= avgMag;
				magJ /= avgMag;
				magO /= avgMag;

				core::vector3df vecIJ(iAngle * angleMultiplier, magJ - magI, 0.0f);
				core::vector3df vecIO(oAngle * angleMultiplier, magO - magI, 0.0f);

				float lengthSQ = vecIJ.getLengthSQ();
				float newValue = 1.0f - vecIJ.dotProduct(vecIO) / lengthSQ;

				if (newValue < 0.0f)
				{
					value = 0.0f;
					break;
				}

				value = core::min_<float>(value, newValue);
			}

			samples[i]->Weight = value;
		}

		float summedWeight = 0;
		for (int i = 0; i < numMotions; i++)
			summedWeight = summedWeight + samples[i]->Weight;

		if (summedWeight > 0)
		{
			for (int i = 0; i < numMotions; i++)
				samples[i]->Weight = samples[i]->Weight / summedWeight;
		}
	}

	void CGradientBandInterpolation::sampleWeightsCartesian(const core::vector3df& vector)
	{
		int numMotions = (int)m_samples.size();
		SSample** samples = m_samples.pointer();

		float totalWeight = 0.0f;

		for (int i = 0; i < numMotions; ++i)
		{
			// Calc vecI -> sample
			core::vector3df pointI = samples[i]->Vector;
			core::vector3df vecIS = vector - pointI;

			float weight = 1.0;

			for (int j = 0; j < numMotions; ++j)
			{
				if (j == i)
					continue;

				// Calc vec i -> j
				core::vector3df pointJ = samples[j]->Vector;
				core::vector3df vecIJ = pointJ - pointI;

				// Calc Weight
				float lenSqIJ = vecIJ.dotProduct(vecIJ);

				float newWeight = vecIS.dotProduct(vecIJ) / lenSqIJ;
				newWeight = 1.0f - newWeight;
				newWeight = core::clamp<float>(newWeight, 0.0, 1.0);

				weight = core::min_<float>(weight, newWeight);
			}

			samples[i]->Weight = weight;

			totalWeight += weight;
		}

		if (totalWeight > 0.0f)
		{
			for (int i = 0; i < numMotions; ++i)
			{
				samples[i]->Weight = samples[i]->Weight / totalWeight;
			}
		}
	}
}