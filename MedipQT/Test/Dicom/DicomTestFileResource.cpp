#include "stdafx.h"
#include "DicomTestFileResource.h"

DicomTestFileResource::DicomTestFileResource()
{
	TestDirPath = GetUnitTestDataDirectory("dicom");
	TestSingleDirPath = TestDirPath + "/single";
	DefaultCT_FilePath = TestSingleDirPath + "/default_ct.dcm";
}

