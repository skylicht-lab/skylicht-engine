#include "pch.h"
#include "CTween.h"
#include "CTweenManager.h"

namespace Skylicht
{
	CTween::CTween() :
		m_time(0.0f),
		m_delay(0.0f),
		m_endDelay(0.0f),
		m_duration(0.0f),
		m_ease(EaseInCubic),
		m_numValue(1),
		m_percentTime(0.0f),
		m_percentValue(0.0f),
		m_start(false)
	{
		m_function = getEasingFunction(m_ease);

		for (int i = 0; i < MAX_TWEEN_VALUE; i++)
		{
			m_fromValue[i] = 0.0f;
			m_toValue[i] = 0.0f;
			m_value[i] = 0.0f;
		}
	}

	CTween::~CTween()
	{

	}

	void CTween::update()
	{
		m_time = m_time + getTimeStep();
		if (m_time < m_delay)
		{
			if (OnDelay != nullptr)
				OnDelay(this);
			return;
		}

		if (m_start == false)
		{
			if (OnStart != nullptr)
				OnStart(this);
			m_start = true;
		}

		if (m_duration > 0.0f)
		{
			float t = m_time - m_delay;
			float f = t / m_duration;

			m_percentTime = core::clamp(f, 0.0f, 1.0f);
			m_percentValue = (float)m_function((double)m_percentTime);

			for (int i = 0; i < m_numValue; i++)
			{
				m_value[i] = m_fromValue[i] + (m_toValue[i] - m_fromValue[i]) * m_percentValue;
			}

			updateValue();

			if (OnUpdate != nullptr)
				OnUpdate(this);

			if (f >= 1.0f)
			{
				float finishTime = t - m_duration;
				if (finishTime >= m_endDelay)
				{
					if (OnFinish != nullptr)
						OnFinish(this);
					CTweenManager::getInstance()->removeTween(this);
				}
				else
				{
					if (OnEndDelay != nullptr)
						OnEndDelay(this);
				}
			}
		}
		else
		{
			for (int i = 0; i < m_numValue; i++)
				m_value[i] = m_toValue[i];

			updateValue();

			if (OnUpdate != nullptr)
				OnUpdate(this);

			if (OnFinish != nullptr)
				OnFinish(this);

			CTweenManager::getInstance()->removeTween(this);
		}
	}

	void CTween::setBeginValue(int index, float value)
	{
		m_fromValue[index] = value;
		if (index >= m_numValue)
			m_numValue = index + 1;
	}

	void CTween::setEndValue(int index, float value)
	{
		m_toValue[index] = value;
		if (index >= m_numValue)
			m_numValue = index + 1;
	}

	void CTween::stop()
	{
		if (OnStop != nullptr)
			OnStop(this);
		CTweenManager::getInstance()->removeTween(this);
	}
}