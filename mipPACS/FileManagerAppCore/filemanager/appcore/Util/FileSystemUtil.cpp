#include "stdafx.h"
#include "FileSystemUtil.h"
#include <qfileinfo>
#include <qdir>

namespace fm
{
	QStringList FileSystemUtil::CopyDirectoryFiles(QString srcDir, QString targetDir, QStringList filters)
	{
		QDir dir(srcDir);
		QFileInfoList fileInfoList = dir.entryInfoList(filters);
		QStringList targetFilePathList;

		for (QFileInfo& fileInfo : fileInfoList)
		{
			QString srcPath = srcDir + "/" + fileInfo.fileName();
			QString targetPath = targetDir + "/" + fileInfo.fileName();

			QFile::copy(srcPath, targetPath);
			targetFilePathList.push_back(targetPath);
		}

		return targetFilePathList;
	}

	QString FileSystemUtil::GetDirPath(QString filepath)
	{
		QFileInfo fileInfo(filepath);
		return fileInfo.dir().absolutePath();
	}

	QString FileSystemUtil::RemoveLastSlash(QString input)
	{
		if (!input.isEmpty())
		{
			if (input.right(1) == "\\" || input.right(1) == "/")
			{
				input = input.left(input.length() - 1);
			}
		}
		return input;
	}

	QString FileSystemUtil::RemoveExtension(QString input)
	{
		int indexOfExtension = input.lastIndexOf(".");
		if (indexOfExtension != -1)
		{
			input = input.mid(0, indexOfExtension);
		}
		return input;
	}

	QString FileSystemUtil::AppendExtension(QString input, QString filter)
	{
		if (!filter.isEmpty())
		{
			QFileInfo fileInfoInput(input);
			QFileInfo fileInfoFilter(filter);

			if (fileInfoInput.suffix() != fileInfoFilter.suffix())
			{
				if (fileInfoFilter.suffix() != "*")
				{
					input += "." + fileInfoFilter.suffix();
				}
			}
		}

		return input;
	}

	std::vector<std::string> FileSystemUtil::GetDirectoryFilePathList(QStringList extensionList, QString dirpath)
	{
		std::vector<std::string> filepathList;
		QFileInfo fileInfo(dirpath);
		if (fileInfo.isDir())
		{
			QDir dir(dirpath);
			QFileInfoList fileInfoList = dir.entryInfoList(extensionList);
			for (QFileInfo& info : fileInfoList)
			{
				filepathList.push_back(info.absoluteFilePath().toStdString());
			}
		}
		return filepathList;
	}

	bool FileSystemUtil::HasExtensionList(QStringList sourceList, QStringList targetList)
	{
		for (QString& source : sourceList)
		{
			for (QString& target : targetList)
			{
				QFileInfo fileInfoSrc(source);
				QFileInfo fileInfoTarget(target);
				QString extSrc = fileInfoSrc.suffix().toLower();
				QString extTarget = fileInfoTarget.suffix().toLower();
				if (extSrc == extTarget)
				{
					return true;
				}
			}
		}
		return false;
	}

}