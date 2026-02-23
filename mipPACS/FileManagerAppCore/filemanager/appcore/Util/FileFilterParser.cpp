#include "stdafx.h"
#include "FileFilterParser.h"

namespace fm
{
	//=====================================
	//	FileFilterInfo
	//=====================================
	FileFilterInfo::FileFilterInfo(QString name, QStringList filterList) :
		m_name(name),
		m_filters(filterList)
	{
	}

	QString FileFilterInfo::GetPresentationName()
	{
		return m_name + "(" + m_filters.join(";") + ")";
	}

	QString FileFilterInfo::Name()
	{
		return m_name;
	}

	QStringList FileFilterInfo::Filters()
	{
		return m_filters;
	}

	//=====================================
	//	FileFilterParser
	//=====================================
	FileFilterParser::FileFilterParser(QString filters) :
		m_filters(filters)
	{
	}

	std::vector<FileFilterInfo> FileFilterParser::Parse()
	{
		std::vector<FileFilterInfo> filterInfos;

		if (!m_filters.isEmpty())
		{
			QStringList fileFilterTokenList = m_filters.split(";;");
			for (QString& fileFilterToken : fileFilterTokenList)
			{
				filterInfos.push_back(ParseFileFilterToken(fileFilterToken));
			}
		}

		return filterInfos;
	}

	FileFilterInfo FileFilterParser::ParseFileFilterToken(QString fileFilterToken)
	{
		SetupFilterToken(fileFilterToken);

		QString nameText;
		QString filterText;
		if (IsFilterHasBraket())
		{
			nameText = fileFilterToken.left(m_startBraketIdx);
			filterText = fileFilterToken.mid(m_startBraketIdx + 1, m_filterTextLength);
		}
		else
		{
			filterText = fileFilterToken;
		}

		QStringList filterList = filterText.split(";");
		return FileFilterInfo(nameText, filterList);
	}

	std::vector<FileFilterInfo> FileFilterParser::Parse(QString filters)
	{
		return FileFilterParser(filters).Parse();
	}

	void FileFilterParser::SetupFilterToken(QString fileFilterToken)
	{
		m_startBraketIdx = fileFilterToken.indexOf("(", 0);
		m_endBraketIdx = fileFilterToken.indexOf(")", 0);
		m_filterTextLength = m_endBraketIdx - m_startBraketIdx - 1;
	}

	bool FileFilterParser::IsFilterHasBraket()
	{
		return (m_endBraketIdx != -1) && (m_endBraketIdx != -1);
	}

}

