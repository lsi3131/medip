#pragma once

#include <qstringlist>
#include <qstring>
#include <vector>
#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT FileFilterInfo
	{
	public:
		FileFilterInfo(QString name, QStringList filterList);

		QString GetPresentationName();
		QString Name();
		QStringList Filters();

	private:
		QString m_name;
		QStringList m_filters;
	};

	class FM_CORE_EXPORT FileFilterParser
	{
	public:
		static std::vector<FileFilterInfo> Parse(QString filters);

	public:
		FileFilterParser(QString filters);

	public:
		std::vector<FileFilterInfo> Parse();

	private:
		FileFilterInfo ParseFileFilterToken(QString fileFilterToken);
		void SetupFilterToken(QString fileFilterToken);
		bool IsFilterHasBraket();


	private:
		QString m_filters;
		int m_startBraketIdx;
		int m_endBraketIdx;
		int m_filterTextLength;
	};
}

