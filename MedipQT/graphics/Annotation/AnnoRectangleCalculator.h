#pragma once

#ifndef ANNORECTANGLECALCULATOR_H
#define ANNORECTANGLECALCULATOR_H

#include <stdint.h>

template <typename T>
class AnnoRectangleCalculator
{
public:
	AnnoRectangleCalculator()
	{
		clear();
	}
public:
	void calc(std::vector<T> dataList, T minRange, T maxRange)
	{
		double sum = 0;
		double dev_pow_sum = 0;
		int count = dataList.size();

		T minValue = maxRange;
		T maxValue = minRange;
		/* Min, Max, Average 계산 */
		for (int i = 0; i < count; ++i)
		{
			if (minValue > dataList[i])
				minValue = dataList[i];

			if (maxValue < dataList[i])
				maxValue = dataList[i];

			sum += dataList[i];
		}
		float average = sum / count;

		/* stdev 계산 */
		for (int i = 0; i < count; ++i)
		{
			dev_pow_sum += pow(dataList[i] - average, 2);
		}
		float variance = dev_pow_sum / count;
		float stdev = sqrt(variance);

		m_mean = average;
		m_min = minValue;
		m_max = maxValue;
		m_standardDeviation = stdev;
		m_area_cm = 0;

		m_isValid = true;
	}
public:
	void clear()
	{
		m_isValid = false;
		m_mean = 0;
		m_standardDeviation = 0;
		m_max = 0;
		m_min = 0;
		m_area_cm = 0;
	}

	bool isValid()
	{
		return m_isValid;
	}
	double mean()
	{
		return m_mean;
	}
	double standardDeviation()
	{
		return m_standardDeviation;
	}
	double max()
	{
		return m_max;
	}
	double min()
	{
		return m_min;
	}
	double area_cm()
	{
		return m_area_cm;
	}

private:
	bool m_isValid;
	double m_mean;
	double m_standardDeviation;
	double m_max;
	double m_min;
	double m_area_cm;
};
#endif