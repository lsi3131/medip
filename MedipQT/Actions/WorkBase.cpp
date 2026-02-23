#include "stdafx.h"
#include "WorkBase.h"

WorkBase::WorkBase() :
	m_progressValue(0)
{
}

void WorkBase::setProgressValue(int value, bool init)
{
	if (init)
	{
		m_progressValue = 0;
		emit progress(m_progressValue);
		return;
	}

	if (100 <= m_progressValue)
	{
		return;
	}

	if ((m_progressValue + 10) <= value)
	{
		m_progressValue = value;
		emit progress(m_progressValue);
	}
}

