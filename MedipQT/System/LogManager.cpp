#include "stdafx.h"
#include "LogManager.h"

#include "LicenseManager.h"
#include "stringManager.h"
#include "ApplicationManager.h"

const qint64 LogManager::LOG_FILES_MAX_LIMIT_SIZE =   104857600;	// 100mb(byte단위)
QString g_strFilePath = "";

LogManager::LogManager(void)
{
	g_strFilePath = makeDateFilePath(APP_MNG->getAppName());
}

LogManager::~LogManager(void)
{

}

const QString LogManager::makeDateFilePath(const QString &strAppName)
{
	QString strDate = QDateTime::currentDateTime().toString("yyyy-MM-dd") + QString(".log");
#ifndef DEV_USE_APPDATA_PATH
	QString configPath = qApp->applicationDirPath();
	configPath += ("/log/");
#else
	QString configPath = STRING_MANAGER->m_strLocalLogPath;
	configPath += ("/");
#endif
//	configPath += strAppName;
//	configPath += ("/");
	QDir logDir(configPath);
	if (!logDir.exists())
		logDir.mkpath(".");
	configPath += strDate;
	return configPath;
}

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	//if (type == QtDebugMsg)
	//	return;

	//Q_UNUSED(context);

	QString dateTime = QDateTime::currentDateTime().toString("hh:mm:ss");
	QString logText = QString("[%1] ").arg(dateTime);
	QString function = context.function;
	QString threadCode;
	threadCode.sprintf("[thread id:0x%x]", QThread::currentThreadId());
	
	bool bFileWrite = false;
	switch (type)
	{
	case QtDebugMsg:
	{
		logText += QString("{Debug}");
#ifdef _DEBUG
		bFileWrite = true;
#else
		bFileWrite = false;
#endif
	}
	break;
	case QtInfoMsg:
	{
		logText += QString("{Info}");
		bFileWrite = true;
	}
	break;
	case QtWarningMsg:
	{
		logText += QString("{Warning}");
		bFileWrite = true;
	}
	break;
	case QtCriticalMsg:
	{
		logText += QString("{Critical}");
		bFileWrite = true;
	}
	break;
	case QtFatalMsg:
	{
		logText += QString("{Fatal}");
		//	abort();
		bFileWrite = true;
	}
	break;
	}

	logText += QString("  func=%1  %2\t%3").arg(function).arg(threadCode).arg(msg);

	if (bFileWrite)
	{
		QFile outFile(g_strFilePath);
		if (outFile.open(QIODevice::Append | QIODevice::Text))
		{
			QTextStream textStream(&outFile);
			textStream << logText << endl;
			outFile.close();
		}
	}
}

void LogManager::installMessageHandler()
{
	qInstallMessageHandler(customMessageHandler);
}

void LogManager::checkLogFiles()
{
	// 환자번호에 해당하는 xml 파일을 찾아서 오픈.
#ifndef DEV_USE_APPDATA_PATH
	QString startDir = qApp->applicationDirPath() + "/log";
#else
	QString startDir = STRING_MANAGER->m_strLocalLogPath;
#endif
	QStringList strFilters;
	strFilters += "*.log";
	QDirIterator iterDir(startDir, strFilters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	qint64 totalFileSize = 0;
	QVector<QString> logFilePathList;
	while (iterDir.hasNext())
	{
		iterDir.next();
		totalFileSize += iterDir.fileInfo().size();
		logFilePathList.push_back(iterDir.filePath());
	//	qDebug() << iterDir.filePath();
	//	qDebug() << iterDir.path();
	//	qDebug() << iterDir.fileInfo().size();
	}
	if (totalFileSize >= LOG_FILES_MAX_LIMIT_SIZE)
	{
		// log 파일 삭제
		for (int i = 0; i < logFilePathList.size(); i++)
		{
			if (QFile::exists(logFilePathList.at(i)))
				QFile::remove(logFilePathList.at(i));
		}
	}
	return;
}

