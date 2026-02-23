#pragma once

#include <QString>

class ActionResult
{
public:
	ActionResult() :
		m_isErrorExist(false)
	{}

public:
	void SetErrorResult(QString title, QString result)
	{
		m_isErrorExist = true;
		m_errorTitle = title;
		m_errorResult = result;
	}

	void Clear()
	{
		m_isErrorExist = false;
		m_errorTitle.clear();
		m_errorResult.clear();
	}

	bool IsErrorExist()
	{
		return m_isErrorExist;
	}

	QString GetErrorTitle()
	{
		return m_errorTitle;
	}

	QString GetErrorResult()
	{
		return m_errorResult;
	}

private:
	bool m_isErrorExist;
	QString m_errorTitle;
	QString m_errorResult;
};

