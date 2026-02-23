#pragma once

#include <QString>
#include <QStringList>

class FolderViewTestStub
{
public:
	FolderViewTestStub();

public:
	QString TestRootDir;
	QString SampleFolder_1;
	QString SampleFolder_2;
	QString SampleFolder_3;
	QString Filepath_dcm;
	QString Filepath_csv;
	QString Filepath_mip;
	QString Filepath_json;
	QString Filepath_obj;
	QString Filepath_png;
	QString Filepath_stl;

	QStringList FilePathList;
};