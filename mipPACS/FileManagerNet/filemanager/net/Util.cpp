#include "stdafx.h"
#include "Util.h"
#include <qfileinfo>

using namespace fm;

std::string FileExtFilterUtil::ToSerialize(QStringList fileOpenFilterList)
{
	std::string text;
	for (QString& str : fileOpenFilterList)
	{
		text += str.toStdString() + ",";
	}

	return text;
}

QStringList FileExtFilterUtil::ToDeserialize(std::string fileOpenFilterText)
{
	QString text(fileOpenFilterText.data());
	QStringList list = text.split(",");
	return list;
}

QString FilePathUtil::ExtractDirectoryPath(QString filepath)
{
	QFileInfo info(filepath);

	if (info.isDir())
	{
		return filepath;
	}

	/* filepath에서 디렉토리 경로만 추출*/
	QString dirpath = filepath;
	int lastIndex = filepath.lastIndexOf("/");
	dirpath = filepath.left(lastIndex);
	return dirpath;
}

QString FilePathUtil::ExtractFileName(QString filepath)
{
	QFileInfo info(filepath);

	if (info.isFile())
	{
		return info.fileName();
	}
	return "";
}

//std::wstring ExtractDirectoryPath(std::wstring filepath)
//{
//	QString qfilepath = QString::fromStdWString(filepath);
//	return ExtractDirectoryPath(qfilepath).toStdWString();
//}
