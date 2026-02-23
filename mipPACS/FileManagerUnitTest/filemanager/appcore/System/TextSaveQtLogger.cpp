#include "pch.h"
#include "TextSaveQtLogger.h"

QStringList TextSaveQtLogger::m_logList;

void TextSaveQtLogger::OnTestMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	TextSaveQtLogger::AddLog(msg);
}

void TextSaveQtLogger::Initialize()
{
	m_logList.clear();
	qInstallMessageHandler(OnTestMessageHandler);
}

void TextSaveQtLogger::AddLog(const QString& msg)
{
	m_logList << msg;
}

QString TextSaveQtLogger::GetLogWithLineFeed()
{
	QString text;
	for (auto& s : m_logList)
	{
		text += s + "\n";
	}
	return text;
}

QStringList TextSaveQtLogger::GetLogList()
{
	return m_logList;
}

void TextSaveQtLogger::Clear()
{
	m_logList.clear();
}

