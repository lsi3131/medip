#include "stdafx.h"
#include "DicomDatasetFactory.h"
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>

namespace fm
{

	std::unique_ptr<DicomDataset> DicomDatasetFactory::CreateQueryLevelDataset(EQueryRetrieveLevel level)
	{
		std::unique_ptr<DicomDataset> pOutDataset = std::make_unique<DicomDataset>();

		if (level == EQueryRetrieveLevel::QR_LEVEL_PATIENT)
		{
			pOutDataset->SetTagValue(DicomTagID::QueryRetrieveLevel, L"PATIENT");
		}
		else if (level == EQueryRetrieveLevel::QR_LEVEL_STUDY)
		{
			pOutDataset->SetTagValue(DicomTagID::QueryRetrieveLevel, L"STUDY");
		}
		else if (level == EQueryRetrieveLevel::QR_LEVEL_SERIES)
		{
			pOutDataset->SetTagValue(DicomTagID::QueryRetrieveLevel, L"SERIES");
		}
		else if (level == EQueryRetrieveLevel::QR_LEVEL_IMAGE)
		{
			pOutDataset->SetTagValue(DicomTagID::QueryRetrieveLevel, L"IMAGE");
		}
		else
		{
			pOutDataset->SetTagValue(DicomTagID::QueryRetrieveLevel, L"SERIES");
		}
		return pOutDataset;
	}

	std::unique_ptr<DicomDataset> DicomDatasetFactory::CreateFindQueryDataset(
		EQueryRetrieveLevel level,
		std::vector<DicomTagID>& findQueryTagIdList,
		std::vector<DicomTagValuePair>& findTagValues
	)
	{
		std::unique_ptr<DicomDataset> pOutDataset = CreateQueryLevelDataset(level);

		/* 기본 Tag 업데이트 */
		for (auto& tagID : findQueryTagIdList)
		{
			if (!pOutDataset->IsTagExists(tagID))
			{
				pOutDataset->SetTagValue(tagID, L"");
			}
		}

		/* Tag ID 값 대입*/
		for (auto& tagID : findQueryTagIdList)
		{
			for (auto& findTagValue : findTagValues)
			{
				if (findTagValue.ID == tagID)
				{
					pOutDataset->SetTagValue(tagID, findTagValue.Value);
					break;
				}
			}
		}

		return pOutDataset;
	}

	std::unique_ptr<DicomDataset> DicomDatasetFactory::CreateStoreDataset()
	{
		std::unique_ptr<DicomDataset> pDataset = std::make_unique<DicomDataset>();
		pDataset->SetTagValue(DicomTagID::SOPClassUID, StringUtil::MultiByteToWide(UID_CTImageStorage));

		char instanceUID[256];
		pDataset->SetTagValue(DicomTagID::SOPInstanceUID, StringUtil::MultiByteToWide(dcmGenerateUniqueIdentifier(instanceUID)));

		return pDataset;
	}
}
