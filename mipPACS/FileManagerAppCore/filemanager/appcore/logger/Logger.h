#pragma once

#include <vector>
#include <qlogging.h>
#include <qdatetime>
#include <qmutex>
#include "filemanager/export.h"

typedef void(*LoggerCallBack)(void* pContext, const char* message);

namespace fm
{
	struct LoggerCallBackInfo
	{
		LoggerCallBack CallBack;
		void* pContext;
	};

	class FM_CORE_EXPORT Logger
	{
	public:
		static bool Initialize();
		static bool Deinitialize();
		static void AddLogFilePath(QString logFilePath);
		static void AddCallBack(LoggerCallBack callBack, void* pContext);

		static void QtLoggerHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);

	private:
		static std::vector<LoggerCallBackInfo> m_loggerCallBackDatas;
		static std::vector<QString> m_logFilePaths;
		static QMutex m_mutex;

	};
};
