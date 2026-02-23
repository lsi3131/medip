#include "stdafx.h"
#include "FolderViewTestStub.h"
#include <qdir>

FolderViewTestStub::FolderViewTestStub()
{
	TestRootDir = QDir::currentPath() + "/TestData/appcore/folderview/";
	SampleFolder_1 = TestRootDir + "sample1";
	SampleFolder_2 = TestRootDir + "sample2";
	SampleFolder_3 = TestRootDir + "sample3";
	Filepath_dcm = TestRootDir + "dcm.dcm";
	Filepath_csv = TestRootDir + "csv.csv";
	Filepath_mip = TestRootDir + "mip.mip";
	Filepath_json = TestRootDir + "json.json";
	Filepath_obj = TestRootDir + "obj.obj";
	Filepath_png = TestRootDir + "png.png";
	Filepath_stl = TestRootDir + "stl.stl";

	FilePathList << SampleFolder_1;
	FilePathList << SampleFolder_2;
	FilePathList << SampleFolder_3;
	FilePathList << Filepath_dcm;
	FilePathList << Filepath_csv;
	FilePathList << Filepath_mip;
	FilePathList << Filepath_json;
	FilePathList << Filepath_obj;
	FilePathList << Filepath_png;
	FilePathList << Filepath_stl;
}
