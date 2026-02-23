#pragma once

#include <qstringlist>
#include <qstring>
#include <string>
#include "filemanager/net/export.h"

namespace fm
{
	class FM_NET_EXPORT FileExtFilterUtil
	{
	public:
		static std::string ToSerialize(QStringList fileOpenFilterList);
		static QStringList ToDeserialize(std::string fileOpenFilterText);


	};

	class FM_NET_EXPORT FilePathUtil
	{
	public:
		/**
			FilePath에서 Direcotry 경로를 반환한다.
			해당 경로가 Directory일 경우 Directory를 반환한다.
		*/
		static QString ExtractDirectoryPath(QString filepath);
		static QString ExtractFileName(QString filepath);
	};
};
