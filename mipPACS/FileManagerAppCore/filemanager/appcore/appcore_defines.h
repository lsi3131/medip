#pragma once

#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/DicomFindOption.h"
#include "filemanager/dicom/DicomDataset.h"
#include <vector>
#include <string>

namespace fm
{
	struct DcmNetFindData
	{
		DicomFindOption FindOption;
		std::vector<DicomDataset> DicomDatasetList;
	};

	struct DcmNetDownloadData
	{
		std::vector<DicomDataset> DicomDatasetList;
	};

	enum WINDOW_TYPE
	{
		WT_AXIAL,
		WT_CORONAL,
		WT_SAGITTAL,
	};

	enum class EPACSOperationMode
	{
		only_search_mode,
		can_download_mode,
	};

	enum class EPACSSearchMode
	{
		search_study,
		search_series
	};

}

