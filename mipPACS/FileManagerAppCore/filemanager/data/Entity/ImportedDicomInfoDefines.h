#pragma once

#include "filemanager/export.h"
#include "CommonDefines.h"

namespace fm
{
	class FM_CORE_EXPORT ImportedDicomInfoDefines
	{
	public:
		static const wchar_t* TABLE_NAME;

		static const wchar_t* RECORD_NAME;

		static const wchar_t* COL_NAME_DICOM_FILE_PATH;
		static const wchar_t* COL_NAME_SERIES_INSTANCE_UID;
		static const wchar_t* COL_NAME_STUDY_INSTANCE_UID;
		static const wchar_t* COL_NAME_PATIENT_NAME;
		static const wchar_t* COL_NAME_DATE_OF_BIRTH;
		static const wchar_t* COL_NAME_SERIES_DATE;
		static const wchar_t* COL_NAME_STUDY_DATE;
		static const wchar_t* COL_NAME_IMPORTED_DATE;
		static const wchar_t* COL_NAME_PATIENT_ID;
		static const wchar_t* COL_NAME_PATIENT_AGE;
		static const wchar_t* COL_NAME_PATIENT_SEX;
		static const wchar_t* COL_NAME_MODALITY;
		static const wchar_t* COL_NAME_STUDY_DESCRIPTION;
		static const wchar_t* COL_NAME_SERIES_DESCRIPTION;
		static const wchar_t* COL_NAME_ACCESSION_NUMBER;
		static const wchar_t* COL_NAME_EXAM_ID;
		static const wchar_t* COL_NAME_REFERRING_PHYSICIAN;
		static const wchar_t* COL_NAME_PERFORMING_PHYSICIAN;
		static const wchar_t* COL_NAME_READING_PHYSICIAN;
		static const wchar_t* COL_NAME_REQUESTING_PHYSICIAN;
		static const wchar_t* COL_NAME_SERIES_NUMBER;
		static const wchar_t* COL_NAME_IMAGE_COUNT;
		static const wchar_t* COL_NAME_SERIES_COUNT_IN_STUDY;
		static const wchar_t* COL_NAME_TAG;
		static const wchar_t* COL_NAME_DICOM_DIRECTORY_PATH;
		static const wchar_t* COL_NAME_DICOM_FILE_LIST;
	};

};
