#pragma once

#include <QStringList>
#include <vector>
#include <string>
#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT FileSystemUtil
	{
	public:
		static QStringList CopyDirectoryFiles(QString srcDir, QString targetDir, QStringList filters);
		static QString GetDirPath(QString filepath);
		static QString RemoveLastSlash(QString input);
		static QString RemoveExtension(QString input);
		static QString AppendExtension(QString input, QString filter);
		static std::vector<std::string> GetDirectoryFilePathList(QStringList extensionList, QString dirpath);
		static bool HasExtensionList(QStringList sourceList, QStringList targetList);

	};
}
