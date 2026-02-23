#pragma once

#include <memory>
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/export.h"

namespace fm
{
	struct DicomTagValuePair
	{
		DicomTagID ID;
		std::wstring Value;

		DicomTagValuePair(DicomTagID tagID, std::wstring value)
		{
			ID = tagID;
			Value = value;
		}
	};

	class FM_CORE_EXPORT DicomDatasetFactory
	{
	public:
		static std::unique_ptr<DicomDataset> CreateQueryLevelDataset(EQueryRetrieveLevel level);

		static std::unique_ptr<DicomDataset> CreateFindQueryDataset(
			EQueryRetrieveLevel level,
			std::vector<DicomTagID>& findQueryTagIdList,
			std::vector<DicomTagValuePair>& findTagValues = std::vector<DicomTagValuePair>()
		);

		static std::unique_ptr<DicomDataset> CreateStoreDataset();


	};
}
