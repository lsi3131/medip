#pragma once

#include "Test/test_pch.h"

class UsdFileTestResource
{
public:
	UsdFileTestResource();

public:
	std::string TestDirPath;
	std::string CubeSTLFilePath;
	std::string CylinderSTLFilePath;
	std::string SphereSTLFilePath;

	std::string TestDirPath_MDL;
	std::string Ceiling_MDL_FilePath;
	std::string Roof_MDL_FilePath;
	std::string Shingles_MDL_FilePath;

	std::string TestDirPath_USD;
	std::string MEDIP_Sample_USD;
	std::string USD_Empty_FilePath;
	std::string TempFilePath_USD;

	std::string PlaneFilePath;
	std::string CubeFilePath;
	std::string SphereFilePath;
	std::string CylinderFilePath;
	std::string ConeFilePath;

	std::string PlaneFilePath_UsdObj;
	std::string CubeFilePath_UsdObj;
	std::string SphereFilePath_UsdObj;
	std::string CylinderFilePath_UsdObj;
	std::string ConeFilePath_UsdObj;

	std::string TestDirPath_Usda;
	std::string MaterialSample_Usda;
	std::string TempFilePath_Usda;

};
