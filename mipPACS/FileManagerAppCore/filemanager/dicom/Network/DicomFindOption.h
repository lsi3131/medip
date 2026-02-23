#pragma once

#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/std/DateTime.h"
#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT DicomFindOption
	{
	public:
		DicomFindOption();
		DicomFindOption(
			EDcmFindMode mode,
			std::wstring text,
			EDcmModality modality,
			EQueryRetrieveLevel qrLevel
		);

		DicomFindOption(
			std::wstring studyInstanceUID
		);

	public:
		void SetDateRange(DateTime start, DateTime end);
		bool HasDateRange();

		bool IsSearchByStudyInstanceUIDMode();

	public:
		EDcmFindMode Mode;
		std::wstring Text;
		EDcmModality Modality;
		EQueryRetrieveLevel QueryRetrieveLevel;
		std::wstring StudyInstanceUID;

	private:
		DateTime m_startDateTime;
		DateTime m_endDateTime;
		bool m_allDate;

	public:
		std::wstring GetDateRange_Text();
	};
}
