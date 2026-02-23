#include "stdafx.h"
#include "Logger.h"
#include <qtextstream>
#include <qfile>

using namespace fm;

std::vector<LoggerCallBackInfo> Logger::m_loggerCallBackDatas;
std::vector<QString> Logger::m_logFilePaths;
QMutex Logger::m_mutex;


bool Logger::Initialize()
{
	qInstallMessageHandler(Logger::QtLoggerHandler);
	return true;
}

bool Logger::Deinitialize()
{
	m_logFilePaths.clear();
	m_loggerCallBackDatas.clear();
	return true;
}

void Logger::AddLogFilePath(QString logFilePath)
{
	m_logFilePaths.push_back(logFilePath);
}

void Logger::AddCallBack(LoggerCallBack callBack, void * pContext)
{
	LoggerCallBackInfo info{ callBack, pContext };
	m_loggerCallBackDatas.push_back(info);
}

void Logger::QtLoggerHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
	m_mutex.lock();
	QString dt = QDateTime::currentDateTime().toString("hh:mm:ss");
	QString txt = QString("[%1] ").arg(dt);
	const char* func = context.function;
	const char* cate = context.category;
	const char* file = context.file;

	switch (type)
	{
	case QtDebugMsg:
	{
		txt += QString("{Debug} %1").arg(msg);
	}
	break;
	case QtInfoMsg:
	{
		txt += QString("{Info} %1").arg(msg);
	}
	break;
	case QtWarningMsg:
	{
		txt += QString("{Warning} %1").arg(msg);
	}
	break;
	case QtCriticalMsg:
	{
		txt += QString("{Critical} %1").arg(msg);
	}
	break;
	case QtFatalMsg:
	{
		txt += QString("{Fatal} %1").arg(msg);
	}
	break;
	}

	for (auto& data : m_loggerCallBackDatas)
	{
		data.CallBack(data.pContext, txt.toLocal8Bit().data());
	}

	for (auto& path : m_logFilePaths)
	{
		QFile file(path);
		if (file.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			QTextStream stream(&file);
			stream << txt << "\n";
			stream.flush();
		}

		file.close();
	}

	m_mutex.unlock();
}

