#pragma once

#ifndef MACRODEFINES_H
#define MACRODEFINES_H

#include <string>

#define MACRO_DUMI_TEST ("dumi")
#define MACRO_START_AND_CLOSE ("start_and_close")
#define MACRO_EXPORT_MASK_RAW ("export_mask_raw")
#define MACRO_EXPORT_MASK_NII ("export_mask_nii")

#define MACRO_OMNIVERSE_UPLOAD_USD ("omniverse_upload_usd")
#define MACRO_OMNIVERSE_CREATE_USD_MODEL ("omniverse_create_usd_model")
#define MACRO_OMNIVERSE_CREATE_MESH_STL ("omniverse_create_mesh_stl")
#define MACRO_OMNIVERSE_TRANSLATE_MESH ("omniverse_translate_mesh")
#define MACRO_OMNIVERSE_SCALE_MESH ("omniverse_scale_mesh")
#define MACRO_OMNIVERSE_ROTATE_MESH ("omniverse_rotate_mesh")
#define MACRO_OMNIVERSE_SET_VISIBLE_MESH ("omniverse_visible_mesh")

struct OmniverseUploadUsdParameter
{
	QString IPAddress;
	QString ServerDirectoryPath;
	QString LocalFilePath;
};

struct OmniverseCreateUsdModelParameter
{
	QString IPAddress;
	QString UsdFilePath;
};

struct OmniverseCreateMeshSTLParameter
{
	QString IPAddress;
	QString UsdFilePath;
	QString LocalSTLFilePath;
	QString MeshLayerName;
};


struct MacroParameter
{
	QString MacroName;
	QString InputMIPFilePath;
	QString ExportDirectory;

	OmniverseUploadUsdParameter OmniverseUploadUsd;
	OmniverseCreateUsdModelParameter OmniverseCreateUsdModelUsd;
	OmniverseCreateMeshSTLParameter OmniverseCreateMeshSTL;
};
#endif
