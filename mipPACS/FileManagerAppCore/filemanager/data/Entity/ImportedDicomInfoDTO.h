#pragma once

#include "filemanager/export.h"
#include "filemanager/std/DateTime.h"
#include "filemanager/data/entity/ImportedDicomInfoDefines.h"
#include "filemanager/dicom/DicomDataset.h"
#include <string>
#include <unordered_map>

namespace fm
{
	std::wstring JoinDicomFileList(const std::vector<std::wstring>& dicomFileList);
	std::vector<std::wstring> SplitDicomFileList(const std::wstring & dicomFileListText);

	class FM_CORE_EXPORT ImportedDicomInfoDTO
	{
	public:
		ImportedDicomInfoDTO();

	public:
		std::wstring DicomFilePath;
		DateTime ImportedDateTime;
		DateTime OpenedDateTime;
		std::wstring Tag;
		std::wstring DicomDirectoryPath;
		std::vector<std::wstring> DicomFileList;

		/* DICOM Header*/

		/* Patient */
		std::wstring PatientID;
		std::wstring PatientName;
		std::wstring PatientSex;
		std::wstring PatientAge;
		DateTime DateOfBirth;

		/* Study */
		std::wstring StudyInstanceUID;
		DateTime StudyDateTime;
		std::wstring StudyDescription;
		std::wstring AccessionNumber;
		std::wstring ExamID;
		std::wstring ReferringPhysician;
		std::wstring PerformingPhysician;
		std::wstring ReadingPhysician;
		std::wstring RequestPhysician;
		int SeriesCountInStudy;

		/* Series */
		std::wstring SeriesInstanceUID;
		DateTime SeriesDateTime;
		std::wstring SeriesNumber;
		std::wstring SeriesDescription;
		std::wstring Modality;
		int ImageCount;

	public:
		void SetData(std::wstring column, std::wstring value);
		std::wstring GetData(std::wstring column);
		bool CompareData(std::wstring column, std::wstring value);
		bool FindData(std::wstring column, std::wstring value);

	public:
		void InitFromDcmDataset(const DicomDataset& dcmDataset);
		DicomDataset ToDcmDataset();
	};

}
