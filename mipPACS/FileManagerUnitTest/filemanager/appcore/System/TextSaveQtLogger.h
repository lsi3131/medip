#pragma once

#include <qlogging.h>
#include <QDebug>

class TextSaveQtLogger
{
public:
	static void OnTestMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

public:
	static void Initialize();
	static void AddLog(const QString& msg);
	static QString GetLogWithLineFeed();
	static QStringList GetLogList();
	static void Clear();

private:
	static QStringList m_logList;
};

