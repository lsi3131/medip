#pragma once

#include <string>
#include "test/test_pch.h"

class DicomTestFileResource
{
public:
	DicomTestFileResource();

public:
	std::string TestDirPath;
	std::string TestSingleDirPath;

	std::string DefaultCT_FilePath;
};
