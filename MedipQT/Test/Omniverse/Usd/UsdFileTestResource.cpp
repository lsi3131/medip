#include "stdafx.h"
#include "UsdFileTestResource.h"

UsdFileTestResource::UsdFileTestResource()
{
	TestDirPath = GetUnitTestDataDirectory("Omniverse");
	CubeSTLFilePath = TestDirPath + "/mesh/cube.stl";
	CylinderSTLFilePath = TestDirPath + "/mesh/cylinder.stl";
	SphereSTLFilePath = TestDirPath + "/mesh/sphere.stl";

	TestDirPath_MDL = TestDirPath + "/mdl";
	Ceiling_MDL_FilePath = TestDirPath_MDL + "/Ceiling_Tiles.mdl";
	Roof_MDL_FilePath = TestDirPath_MDL + "/Roof_Tiles.mdl";
	Shingles_MDL_FilePath = TestDirPath_MDL + "/Shingles_01.mdl";

	TestDirPath_USD = TestDirPath + "/usd";
	MEDIP_Sample_USD = TestDirPath_USD + "/medip_sample.usd";
	USD_Empty_FilePath = TestDirPath_USD + "/empty.usd";

	PlaneFilePath = TestDirPath_USD + "/plane.usd";
	CubeFilePath = TestDirPath_USD + "/cube.usd";
	SphereFilePath = TestDirPath_USD + "/sphere.usd";
	CylinderFilePath = TestDirPath_USD + "/cylinder.usd";
	ConeFilePath = TestDirPath_USD + "/cone.usd";

	PlaneFilePath_UsdObj = TestDirPath_USD + "/plane_obj.usd";
	CubeFilePath_UsdObj = TestDirPath_USD + "/cube_obj.usd";
	SphereFilePath_UsdObj = TestDirPath_USD + "/sphere_obj.usd";
	CylinderFilePath_UsdObj = TestDirPath_USD + "/cylinder_obj.usd";
	ConeFilePath_UsdObj = TestDirPath_USD + "/cone_obj.usd";

	TestDirPath_Usda = GetUnitTestDataDirectory("Omniverse/usda").c_str();
	MaterialSample_Usda = TestDirPath_Usda + "/material_sample.usda";
	TempFilePath_Usda = TestDirPath_Usda + "/temp.usda";
	TempFilePath_USD = TestDirPath_Usda + "/temp.usd";
}

