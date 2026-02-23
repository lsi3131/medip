#include "stdafx.h"
#include "LastEditedFileDTO.h"
#include "LastEditedFileDefines.h"

void fm::LastEditedFileDTO::SetData(std::wstring column, std::wstring value)
{
	if (column == LastEditedFileDefines::COL_NAME_FILE_PATH)
	{
		FilePath = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_EDIT_TIME)
	{
		EditTime.SetFormattedText(StringUtil::WideToMulitiByte(value));
	}
	else if (column == LastEditedFileDefines::COL_NAME_SERIES_INSTANCE_UID)
	{
		SeriesInstanceUID = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_STUDY_INSTANCE_UID)
	{
		StudyInstanceUID = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_PATIENT_NAME)
	{
		PatientName = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_DATE_OF_BIRTH)
	{
		DateOfBirth = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_PATIENT_ID)
	{
		PatientID = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_MODALITY)
	{
		Modality = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_STUDY_DESCRIPTION)
	{
		StudyDescription = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_SERIES_DESCRIPTION)
	{
		SeriesDescription = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_ACCESSION_NUMBER)
	{
		AccessionNumber = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_EXAM_ID)
	{
		ExamID = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_REFERRING_PHYSICIAN)
	{
		ReferringPhysician = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_PERFORMING_PHYSICIAN)
	{
		PerformingPhysician = value;
	}
	else if (column == LastEditedFileDefines::COL_NAME_READING_PHYSICIAN)
	{
		ReadingPhysician = value;
	}
}

std::wstring fm::LastEditedFileDTO::GetData(std::wstring column)
{
	if (column == LastEditedFileDefines::COL_NAME_FILE_PATH)
	{
		return FilePath;
	}
	else if (column == LastEditedFileDefines::COL_NAME_EDIT_TIME)
	{
		return StringUtil::MultiByteToWide(EditTime.ToFormatText_DateTime());
	}
	else if (column == LastEditedFileDefines::COL_NAME_SERIES_INSTANCE_UID)
	{
		return SeriesInstanceUID;
	}
	else if (column == LastEditedFileDefines::COL_NAME_STUDY_INSTANCE_UID)
	{
		return StudyInstanceUID;
	}
	else if (column == LastEditedFileDefines::COL_NAME_PATIENT_NAME)
	{
		return PatientName;
	}
	else if (column == LastEditedFileDefines::COL_NAME_DATE_OF_BIRTH)
	{
		return DateOfBirth;
	}
	else if (column == LastEditedFileDefines::COL_NAME_PATIENT_ID)
	{
		return PatientID;
	}
	else if (column == LastEditedFileDefines::COL_NAME_MODALITY)
	{
		return 	Modality;
	}
	else if (column == LastEditedFileDefines::COL_NAME_STUDY_DESCRIPTION)
	{
		return StudyDescription;
	}
	else if (column == LastEditedFileDefines::COL_NAME_SERIES_DESCRIPTION)
	{
		return SeriesDescription;
	}
	else if (column == LastEditedFileDefines::COL_NAME_ACCESSION_NUMBER)
	{
		return AccessionNumber;
	}
	else if (column == LastEditedFileDefines::COL_NAME_EXAM_ID)
	{
		return ExamID;
	}
	else if (column == LastEditedFileDefines::COL_NAME_REFERRING_PHYSICIAN)
	{
		return ReferringPhysician;
	}
	else if (column == LastEditedFileDefines::COL_NAME_PERFORMING_PHYSICIAN)
	{
		return PerformingPhysician;
	}
	else if (column == LastEditedFileDefines::COL_NAME_READING_PHYSICIAN)
	{
		return ReadingPhysician;
	}
	return L"";
}
